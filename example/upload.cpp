#include <iostream>

#include <CL/cl.hpp>

#include <EasyCl.h>

using namespace std;

int main()
{
	EasyCl::DeviceManager deviceManager;
	EasyCl::ComputeDevice* device = deviceManager.defaultDevice(CL_DEVICE_TYPE_ALL);

	if(device == NULL)
	{
		cout << "No device avliable. exiting" << endl;
		exit(0);
	}

	cout << "selected device:" << endl;
	cout << "Name:\t\t" << device->device.getInfo<CL_DEVICE_NAME>() << endl;
	cout << "OpenCL version:\t" << device->device.getInfo<CL_DEVICE_VERSION>() << endl;

	EasyCl::SourceCode sourceCode("kernels/upload.cl");
	if(sourceCode.good() == false)
		exit(0);
	//cout << sourceCode.code << endl;

	EasyCl::Software program(device, sourceCode);
	if(program.build() != CL_SUCCESS)
		exit(0);

	EasyCl::Kernel upload(program, "upload");
	EasyCl::Kernel copy(program, "copy");

	int data[16] = {0};
	int reslut[16] = {0};
	int index = 0;
	int ind = 0;
	for(int i=0;i<16;i++)
		data[i] = 59;

	program.createBuffer(CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,128*sizeof(int),NULL,ind);
	upload.setArg(0,*program.buffers[0]);

	upload.setArgBuffer(1,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,16*sizeof(int),data);
	upload.setArgBuffer(2,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(int),&index);
	upload.enqueueTask();

	copy.setArgBuffer(0,CL_MEM_WRITE_ONLY|CL_MEM_COPY_HOST_PTR,16*sizeof(int),NULL);
	copy.setArg(1,*program.buffers[0]);

	cl::NDRange global(30);
	cl::NDRange local(1);
	copy.enqueueNDRange(cl::NullRange, global, local);

	copy.readBuffer(0,16*sizeof(int),reslut);
	for(int i=0;i<16;i++)
		cout << reslut[i] << " ";
	cout << endl;


	return 0;
}
