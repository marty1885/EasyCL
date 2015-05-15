#include <iostream>

#include <CL/cl.hpp>
#include <EasyCl.h>

using namespace EasyCl;
using namespace std;

//////////////////////////////////////////////////////////////////////////
//DeviceManager								//
//////////////////////////////////////////////////////////////////////////

SourceCode::SourceCode(std::string path)
{
	load(path);
}

SourceCode::SourceCode()
{
}

int SourceCode::load(std::string path)
{
	isGood = false;

	std::ifstream file(path);
	if(file.good() == false)
	{
		cout << "Failed to load " << path << " ." << endl;
		return 0;
	}

	std::string sourceCode(
            std::istreambuf_iterator<char>(file),
            (std::istreambuf_iterator<char>()));

	source = cl::Program::Sources(1, std::make_pair(sourceCode.c_str(), sourceCode.length()+1));
	code = sourceCode;
	isGood = true;

	return 1;
}

bool SourceCode::good()
{
	return isGood;
}

//////////////////////////////////////////////////////////////////////////
//ComputeDevice								//
//////////////////////////////////////////////////////////////////////////

ComputeDevice::ComputeDevice(cl::Device dev,cl::Context con)
{
	device = dev;
	context = con;
	commandQueue = cl::CommandQueue(context, device);
}

//////////////////////////////////////////////////////////////////////////
//ComputeDeviceList							//
//////////////////////////////////////////////////////////////////////////

bool ComputeDevice::avliable()
{
	int inQueueComunt = commandQueue.getInfo<CL_QUEUE_REFERENCE_COUNT>();
	if(inQueueComunt == 0)
		return false;
	return true;
}

ComputeDevice* ComputeDeviceList::defaultDevice(cl_device_type deviceType)
{
	if(deviceType == CL_DEVICE_TYPE_ALL)
		return &devices[0];

	int deviceCount = devices.size();
	for(int i=0;i<deviceCount;i++)
	{
		cl_device_type type = devices[i].device.getInfo<CL_DEVICE_TYPE>();
		//NOTE: I shouldn't need deviceType == CL_DEVICE_TYPE_ALL here. Just for completeness.
		if((type & deviceType) != 0 || deviceType == CL_DEVICE_TYPE_ALL)
			return &devices[i];
	}

	return NULL;
}

ComputeDeviceList ComputeDeviceList::findDevice(std::string keyWord,cl_device_type deviceType)
{
	int deviceCount = devices.size();
	ComputeDeviceList list;
	for(int i=0;i<deviceCount;i++)
	{
		cl_device_type type = devices[i].device.getInfo<CL_DEVICE_TYPE>();
		if((type & deviceType) || deviceType == CL_DEVICE_TYPE_ALL)
			if(devices[i].device.getInfo<CL_DEVICE_NAME>().find(keyWord) != std::string::npos)
				list.devices.push_back(devices[i]);
	}

	return list;
}

//////////////////////////////////////////////////////////////////////////
//ComputeDevice								//
//////////////////////////////////////////////////////////////////////////

Software::Software(ComputeDevice* dev, SourceCode sou)
{
	//cl_int err = this->errorCode;
	device = dev;
	sourceCode = sou;

	program = cl::Program(device->context, sourceCode.source,&errorCode);
	if(errorCode != CL_SUCCESS)
	{
		cout << "Error while creating Software/cl::Program , code " << errorCode << endl;
		isGood = false;
	}
	else
		isGood = true;
}

Software::Software()
{
}

Software::~Software()
{
	int size = buffers.size();
	for(int i=0;i<size;i++)
		if(buffers[i] != NULL)
			delete buffers[i];
}

cl_int Software::build(string options)
{
	//HACK: This is a nasty way to make cl::Program::build() only build for one device
	std::vector<cl::Device> devs;
	devs.push_back(device->device);

	cl_int result = program.build(devs,options.c_str());
	if(result != CL_SUCCESS)
	{
		cl_int err = 0;
		//NOTE 2015/5/9 : nVidia OpenCL compiler SUCKS. the error message is mostly misleading.
		//Go for AMD or Intel if possible. Really.
		cout << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devs[0],&err) << endl;

		cout << "Error occured while compiling OpenCL program" << endl
			<< "Error Code: " << result << endl;
		return result;
	}
	return result;
}

bool Software::good()
{
	return isGood;
}

cl_int Software::getError()
{
	return errorCode;
}

cl_int Software::createBuffer(cl_mem_flags flags, size_t size, void* ptr,int& index)
{
	int buffersListSize = buffers.size();
	int emptyIndex = -1;
	for(int i=0;i<buffersListSize;i++)
		if(buffers[i] == NULL)
		{
			emptyIndex = i;
			break;
		}

	cl_int err = 0;
	cl::Buffer* newBuffer = new cl::Buffer(device->context,flags,size,&err);
	if(err != CL_SUCCESS)
	{
		cout << "Error creating cl::Buffer , code " << err << endl;
		return err;
	}
	if(ptr != NULL)
	{
		err = device->commandQueue.enqueueWriteBuffer(*newBuffer,CL_TRUE,0,size, ptr);
		if(err != CL_SUCCESS)
		{
			cout << "Error uploading data to device , code " << err << endl;
			delete newBuffer;
			return err;
		}
	}

	if(emptyIndex >= 0)
	{
		buffers[emptyIndex] = newBuffer;
		index = emptyIndex;
	}
	else
	{
		index = buffers.size();
		buffers.push_back(newBuffer);
	}
	return err;

}

void Software::releaseBuffer(int index)
{
	delete buffers[index];
	buffers[index] = NULL;
}

//////////////////////////////////////////////////////////////////////////
//DeviceManager								//
//////////////////////////////////////////////////////////////////////////

Kernel::Kernel(Software program, std::string funcName)
{
	software = program;
	cl_int err = 0;

	kernel = cl::Kernel(software.program, funcName.c_str(),&err);

	if(err != CL_SUCCESS)
	{
		cout << "Error creating kernel " << funcName << ", code " << err << endl;
		return;
	}

	int argCount = kernel.getInfo<CL_KERNEL_NUM_ARGS>();
	buffers = new cl::Buffer* [argCount];
	delProtect = new bool[argCount];
	for(int i=0;i<argCount;i++)
		buffers[i] = NULL, delProtect[i] = false;
}

Kernel::~Kernel()
{
	int argCount = kernel.getInfo<CL_KERNEL_NUM_ARGS>();
	for(int i=0;i<argCount;i++)
		if(buffers[i] != NULL && delProtect[i] == false)
			delete buffers[i];
	delete [] buffers;
	delete [] delProtect;
}

cl_int Kernel::setArgBuffer(int index, cl_mem_flags flags, size_t size, void* ptr)
{
	if(buffers[index] != NULL && delProtect[index] == false)
		delete buffers[index];

	cl_int err = 0;
	buffers[index] = new cl::Buffer(software.device->context, flags,size,&err);

	if(err != CL_SUCCESS)
	{
		cout << "Error creating cl::Buffer , code " << err << endl;
		return err;
	}

	if(ptr != NULL)
	{
		err = software.device->commandQueue.enqueueWriteBuffer(*buffers[index],CL_TRUE,0,size, ptr);
		if(err != CL_SUCCESS)
		{
			cout << "Error uploading data to device , code " << err << endl;
			//delete buffers[index];
			return err;
		}
	}


	err = kernel.setArg(index, *buffers[index]);
	if(err != CL_SUCCESS)
	{
		cout << "Error set argumant to kernel , code " << err << endl;
		delete buffers[index];
		return err;
	}
	delProtect[index] = false;

	return err;
}

cl_int Kernel::setArg(int index, cl::Buffer& buf)
{
	if(buffers[index] != NULL && delProtect[index] == false)
		delete buffers[index];
	delProtect[index] = true;
	buffers[index] = &buf;
	cl_int err = kernel.setArg(index,buf);
	if(err != CL_SUCCESS)
	{
		cout << "Error set argumant to kernel , code " << err << endl;
		return err;
	}
	return err;
}

cl_int Kernel::enqueueNDRange(cl::NDRange offset, cl::NDRange global, cl::NDRange local)
{
	cl_int err = 0;
	cl::Event event;
	err = software.device->commandQueue.enqueueNDRangeKernel(kernel,offset,global,local,NULL,&event);
	if(err != CL_SUCCESS)
	{
		cout << "Error enqueuing NDRange Kernel, code " << err << endl;
		return err;
	}
	event.wait();
	return err;
}

cl_int Kernel::enqueueTask()
{
	cl_int err = 0;
	cl::Event event;
	err = software.device->commandQueue.enqueueTask(kernel,NULL,&event);
	if(err != CL_SUCCESS)
	{
		cout << "Error enqueuing Task, code " << err << endl;
		return err;
	}
	event.wait();
	return err;
}

cl_int Kernel::enqueueSPMD()
{
	cl_int err = 0;
	int preferWorkSizeBase =
		kernel.getWorkGroupInfo<CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE>
		(software.device->device);
	int maxComputeUnit = software.device->device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
	int spmdSize = preferWorkSizeBase*maxComputeUnit;
	cl::NDRange global(spmdSize);
	cl::NDRange local(1);
	cl::Event event;
	err = software.device->commandQueue.enqueueNDRangeKernel(kernel,cl::NullRange,global,local,NULL,&event);
	if(err != CL_SUCCESS)
	{
		cout << "Error enqueuing NDRange Kernel, code " << err << endl;
		return err;
	}
	event.wait();
	return err;

}

cl_int Kernel::readBuffer(int index, size_t size, void* ptr)
{
	cl_int err = 0;
	err = software.device->commandQueue.enqueueReadBuffer(*buffers[index],CL_TRUE, 0, size, ptr);
	if(err != CL_SUCCESS)
	{
		cout << "Error reading bubber " << index << ", code " << err << endl;
		return err;
	}
	return err;
}

//////////////////////////////////////////////////////////////////////////
//DeviceManager								//
//////////////////////////////////////////////////////////////////////////

DeviceManager::DeviceManager()
{
	vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	int platformCount = platforms.size();

	for(int i=0;i<platformCount;i++)
	{
		cl_context_properties properties[] =
			{ CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[i])(), 0};
		cl::Context context(CL_DEVICE_TYPE_ALL, properties);
		std::vector<cl::Device> platformDevice = context.getInfo<CL_CONTEXT_DEVICES>();
		ComputeDeviceList deviceList;
		int deviceCount = platformDevice.size();

		for(int j=0;j<deviceCount;j++)
		{
			ComputeDevice device(platformDevice[j],context);
			deviceList.devices.push_back(device);
		}

		deviceLists.push_back(deviceList);
	}
}

ComputeDevice* DeviceManager::defaultDevice(cl_device_type deviceType)
{
	int platformCount = deviceLists.size();

	for(int i=0;i<platformCount;i++)
	{
		ComputeDevice* device = deviceLists[i].defaultDevice(deviceType);
		if(device != NULL)
			return device;
	}

	return NULL;
}

ComputeDeviceList DeviceManager::findDevice(std::string keyWord,cl_device_type deviceType)
{
	ComputeDeviceList list;

	int platformCount = deviceLists.size();
	for(int i=0;i<platformCount;i++)
	{
		ComputeDeviceList found = deviceLists[i].findDevice(keyWord,deviceType);
		if(found.devices.size() != 0)
			list.devices.insert(list.devices.end(), found.devices.begin(), found.devices.end());
	}
	return list;
}
