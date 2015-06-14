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

	EasyCl::SourceCode sourceCode("kernels/upload.cl");
	if(sourceCode.good() == false)
		exit(0);
	//cout << sourceCode.code << endl;

	EasyCl::Software program(device, sourceCode);
	if(program.build() != CL_SUCCESS)
		exit(0);

	EasyCl::Kernel upload(program, "upload");
	EasyCl::Kernel copy(program, "copy");

	const int uploadSize = 0xffff;
	int* data = new int[uploadSize];
	int* reslut = new int[uploadSize];;
	int index = 0;
	int ind = 0;
	for(int i=0;i<uploadSize;i++)
		data[i] = 0;

	program.createBuffer(CL_MEM_READ_WRITE|CL_MEM_ALLOC_HOST_PTR,uploadSize*sizeof(int),NULL,ind);
	upload.setArg(0,*program.buffers[ind]);

	upload.setArgBuffer(1,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,128*sizeof(int),data);
	upload.setArgBuffer(2,CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,sizeof(int),&index);
	upload.enqueueTask();

	copy.setArgBuffer(0,CL_MEM_WRITE_ONLY|CL_MEM_ALLOC_HOST_PTR,uploadSize*sizeof(int),NULL);;
	copy.setArg(1,*program.buffers[ind]);

	cl::NDRange global(uploadSize);
	cl::NDRange local(1);
	copy.enqueueNDRange(cl::NullRange, global, local);

	copy.readBuffer(0,uploadSize*sizeof(int),reslut);
	/*for(int i=0;i<0xffff;i++)
		cout << reslut[i] << " ";
	cout << endl;*/


	return 0;
}
