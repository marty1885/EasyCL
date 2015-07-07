#include <iostream>

#include <CL/cl.hpp>

#include <EasyCl.h>

using namespace std;

int main()
{
	EasyCl::DeviceManager deviceManager;
	EasyCl::ComputeDevice* device = deviceManager.defaultDevice(CL_DEVICE_TYPE_GPU);
					//deviceManager.findDevice("HD",CL_DEVICE_TYPE_GPU).devices[0];

	if(device == NULL)
	{
		cout << "No device avliable. exiting" << endl;
		exit(0);
	}

	cout << "selected device:" << endl;
	cout << "Name:\t\t" << device->device.getInfo<CL_DEVICE_NAME>() << endl;
	cout << "OpenCL version:\t" << device->device.getInfo<CL_DEVICE_VERSION>() << endl;

	EasyCl::SourceCode sourceCode("kernels/simple.cl");
	if(sourceCode.good() == false)
		exit(0);
	//cout << sourceCode.source << endl;

	EasyCl::Software program(device, sourceCode);
	if(program.build() != CL_SUCCESS)
		exit(0);

	EasyCl::Kernel kernel(program, "neatStuff");
	int data[30] = {0};
	int copy[30] = {50};

	kernel.setArgBuffer(0,CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,30*sizeof(int),copy);
	kernel.setArgBuffer(1,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,30*sizeof(int),data);

	cl::NDRange global(30);
	cl::NDRange local(1);
	kernel.enqueueNDRange(cl::NullRange, global, local);

	kernel.readBuffer(0,30*sizeof(int),copy);
	for(int i=0;i<30;i++)
		cout << copy[i] << " ";
	cout << endl;

	return 0;
}
