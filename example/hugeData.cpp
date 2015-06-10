#include <iostream>

#include <CL/cl.hpp>

#include <EasyCl.h>

using namespace std;

int main()
{
	EasyCl::DeviceManager deviceManager;
	EasyCl::ComputeDevice* device = deviceManager.defaultDevice(CL_DEVICE_TYPE_GPU);

	if(device == NULL)
	{
		cout << "No device avliable. exiting" << endl;
		exit(0);
	}

	cout << "selected device:" << endl;
	cout << "Name:\t\t" << device->device.getInfo<CL_DEVICE_NAME>() << endl;
	cout << "OpenCL version:\t" << device->device.getInfo<CL_DEVICE_VERSION>() << endl;

	EasyCl::SourceCode sourceCode("kernels/hugeData.cl");
	if(sourceCode.good() == false)
		exit(0);
	//cout << sourceCode.code << endl;

	EasyCl::Software program(device, sourceCode);
	if(program.build() != CL_SUCCESS)
		exit(0);

	EasyCl::Kernel kernel(program, "porcess");
	char* buffer = new char[1920*1080*3];

	kernel.setArgBuffer(0,CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,1920*1080*3,buffer);

	cl::NDRange global(30);
	cl::NDRange local(1);
	kernel.enqueueNDRange(cl::NullRange, global, local);

	//kernel.readBuffer(0,30*sizeof(int),copy);

	delete [] buffer;

	return 0;
}
