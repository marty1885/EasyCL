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

	EasyCl::SourceCode sourceCode("kernels/reallySmallRSACrack.cl");
	if(sourceCode.good() == false)
		exit(0);
	//cout << sourceCode.code << endl;

	EasyCl::Software program(device, sourceCode);
	if(program.build() != CL_SUCCESS)
		exit(0);

	EasyCl::Kernel kernel(program, "reallySmallRSACrack");
	//modulus N
	int RSAN = 571*283;
	int result[2] = {0};
	int communicateBuffer = 0;

	kernel.setArgBuffer(0,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,sizeof(int),&RSAN);
	kernel.setArgBuffer(1,CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,2*sizeof(int),NULL);
	kernel.setArgBuffer(2,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,sizeof(int),&communicateBuffer);

	kernel.enqueueSPMD();

	kernel.readBuffer(1,2*sizeof(int),result);
	cout << "RAS-N = " << RSAN << endl;
	cout << "p = " << result[0] << ", q = " << result[1] << endl;

	return 0;
}
