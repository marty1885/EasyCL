#ifndef EASYCL_H
#define EASYCL_H

#include <CL/cl.hpp>

#include <vector>
#include <string>
#include <fstream>

namespace EasyCl
{

class SourceCode
{
public:
	SourceCode(std::string path);
	SourceCode();
	int load(std::string path);
	bool good();

	std::string code;
	cl::Program::Sources source;
	bool isGood = false;
};

class ComputeDevice
{
public:
	ComputeDevice(cl::Device dev,cl::Context con);
	bool avliable();

	cl::Device device;
	cl::Context context;
	cl::CommandQueue commandQueue;
};

class Software//XXX:This is a bad name. But I can come up with a better one.
{
public:
	Software(ComputeDevice* dev, SourceCode sou);
	Software();
	~Software();
	cl_int build(std::string options = "");

	cl_int createBuffer(cl_mem_flags flags, size_t size, void* ptr, int& index);
	void releaseBuffer(int index);

	ComputeDevice* device;
	SourceCode sourceCode;
	cl::Program program;
	std::vector<cl::Buffer*> buffers;
};

class Kernel
{
public:
	Kernel(Software program, std::string funcName);
	~Kernel();
	cl_int setArgBuffer(int index, cl_mem_flags flags, size_t size, void* ptr);
	cl_int setArg(int index, cl::Buffer& buf);
	cl_int enqueueNDRange(cl::NDRange offset, cl::NDRange global, cl::NDRange local);
	cl_int enqueueTask();
	cl_int enqueueSPMD();
	cl_int readBuffer(int index, size_t size, void* ptr);

	Software software;
	cl::Kernel kernel;
	cl::Buffer** buffers = NULL;
	bool* delProtect = NULL;
};

class ComputeDeviceList
{
public:
	std::vector<ComputeDevice> devices;

	ComputeDevice* defaultDevice(cl_device_type deviceType = CL_DEVICE_TYPE_ALL);
	ComputeDeviceList findDevice(std::string keyWord,cl_device_type deviceType = CL_DEVICE_TYPE_ALL);
};

class DeviceManager
{
public:
	std::vector<ComputeDeviceList> deviceLists;

	DeviceManager();
	ComputeDevice* defaultDevice(cl_device_type deviceType = CL_DEVICE_TYPE_ALL);
	ComputeDeviceList findDevice(std::string keyWord,cl_device_type deviceType = CL_DEVICE_TYPE_ALL);
};

};
#endif
