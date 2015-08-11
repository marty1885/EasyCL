# EasyCL
EasyCL - a library let you access the power of OpenCL easily. <br>
NOTE : EasyCL does not support images now. But is planned in the near future.<br>
NOTE : There is currently no plain on supporting OpenGL/Direct3D data access.<br>
NOTE : Please run doxygen your self. I don't have a good approach to do that in CMake. Sorry.<br>

##What it does
Basiclly EasyCL is a library that enables you to build and lunch a OpenCL program in just a few lines of code.<br><br>
`OpenCL or Open Compute Language is the first open, royalty-free standard for cross-platform, parallel programming of modern processors found in personal computers, servers and handheld/embedded devices. OpenCL greatly improves speed and responsiveness for a wide spectrum of applications in numerous market categories from gaming and entertainment to scientific and medical software.`<br>
-Khronos<br>
<br>
Although OpenCL is powerful, it's acuatlly a pritty triing job to write the host application code. To solve this issue. EasyCL is created.<br>
<br>
EasyCL is designed to enable you to launch OpenCL program in just a few line of code. Whith error/ warrning handler equiped.<br>
<br>
EasyCL is released under the MIT license, so you can use it in open source or commercial code. Details will be in the LICENSE file in this repo.<br>
<br>
EasyCL is guarantee to be able to be compiled on GCC/clang with the proper arguments and envitoment setted.<br>
<br>

##What it does NOT
1.There is currently no plain on supporting OpenGL/Direct3D data access.<br>
2.No guarantee to be able to be compiled on MSVC/Intel CC. Hope it can. If anyone needs it on those compilers. Send me a mail or post a issue. I'll do my best to help you.<br>
<br>
##Show me some code
####The header 
```C++
#include <EasyCl.h>
```

####Get a OpenCL device
```C++
	EasyCl::DeviceManager deviceManager;
	EasyCl::ComputeDevice* device = deviceManager.defaultDevice(CL_DEVICE_TYPE_GPU);
```
EasyCl::DeviceManager::defaultDevice returns the first device EasyCL finds in the selected category, <br>
By changine `CL_DEVICE_TYPE_GPU` into `CL_DEVICE_TYPE_CPU` or `CL_DEVICE_TYPE_ACCLERAOR`, it will return  devices in dirrerent category.<br>
You can also do something like `CL_DEVICE_GPU|CL_DEVICE_CPU` to search in muiltple categories.<br>

####Loading OpenCL source code.
```C++
	EasyCl::SourceCode sourceCode("someOpenCLCode.cl");
```
It's just easy as that<br>
There is also `bool EasyCl::SourceCode::good()` that tells you weather the loading is success.

####Building program
In OpenCL, after loading your source code. What you wana to do next is to build a program.(compile OpenCL code)<br>
```C++
	EasyCl::Software program(device, sourceCode);
	program.build();
```
`int EasyCL::Software::build(std::string options)` returns error code. If it returns `CL_SUCCESS` than the build is success.<br>
`build()` also take a "option" argument. This is the OpenCL compilation argument. "option" will be driectly passed to OpenCL. EasyCL does not process/use it.<br>
To add options to the building process. just `program.build(<args here>);`<br>
Ex : `program.build("-cl-denorms-are-zero -DOCL_KERNEL");`<br>

####Create a kernel
After building a program, inorder to run it. We have to create what's called a Kernel. A Kernel is a speacial function declared with the tag `__kernel` and mosu return `void`. I'lll not go to deep here since this is EasyCL's README not a OpenCL tutorial.<br>
<br>
So, hot to crate a Kernl in EasyCL? Well...<br>
```C++
	EasyCl::Kernel kernel(program, "kernlNameHere");
```

####Setting kernl arguments(pass data to kernl)
To set a Kernel's argument. just do<br>
```C++
	kernel.setArgBuffer(0,CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,30*sizeof(int),data);
```
it's declartion is
```C++
	cl_int Kernel::setArgBuffer(int index, cl_mem_flags flags, size_t size, void* ptr);
```

where index is the index of whtch argumrnt you want to set on the kernl<br>
flags is how you want the memory can be accessed. The value is builed up with 2 parts. First reead/write fags, than allocation flags. The r/w flag can be `CL_MEM_WRITE_ONLY`, `CL_MEM_READ_ONLY` or `CL_MEMREAD__WRITE`. The	allocation flag can be `CL_MEM_USE_HOST_PTR`, `CL_MEM_USE_COPY_PTR`, `CL_MEM_ALLOC_HOST_PTR`.<br>
size is how big the gargument you want to pass is in bytes.<br>
ptr is the pointer to the argumant you want to pass.<br>
<br>
NOTE : ptr can also be `NULL` to create a blank memory for OpenCL. Please notice that if ptr = NULL, than flags must contain `CL_MEM_ALLOC_HOST_PTR` or else it will crash.<br>
<br>
####Launching Kernel
there are three modes EasyCL and launch a kernel.<br>
#####NDRange (N Dimention Range)
First, the classic NDRange mode. I'm not going to explain that is and how NDRange works here. For more infomation, please visit here [NDRange-Explain].<br>
To launch a NDRange Kernel. It's very easy. just few lines will do the job.<br>
```C++
	cl::NDRange global(30);
	cl::NDRange local(1);
	cl::NDRange offset;
	kernel->enqueueNDRange(offset, global, local);
```

#####Task
A Task means to launch a single threaded Kernel. It's equivalent as `global = 1, loca0 = 1, offset = 0` than do a NDRange. It is NOT paralled.<br>
To launch a task, please do the flowing
```C++
	kernl.enqueueTask();
```

#####SPMP
This is a mode I created in EasyCL. If you are fimilar with OpenCL : It launches a work-item for each processing element. It's equivalent to `global = PROCESSING_ELEMENT_COUNT, local = PREFERED_WORKGROUPE_BASE, offset = 0` than launch a 1D NDRange.<br>
Or, in layman's terms : It give every core on your device a thread.<br>
NOTE : SPMD stands for Single Program Muilt data. Not Single Prodessor Multi Data.
To do a SPMD, do the flowing
```C++
	kernl.enqueueSPMD();
```
####Reading data back.
After dong some computing , you will want to read the result back. To do that, puease do
```C++
		kernel->readBuffer(0,30*sizeof(int),copy);
```
which is decleared as
```C++
	cl_int Kernel::readBuffer(int index, size_t size, void* ptr)
```
which index is the index of the argument you want to read of the Kernl<br>
size is how much data you want to read back in bytes.<br>
ptr is where you want the data to be read to.<br>
`readBuffer` retuens a error code. if it returns `CL_SUCCESS` than everting in fine. Else, something is wrong.

##Sample Code
This is a simple code of EasyCL in action. more can be find in the example folder in this repo.<br>
`example.cpp`
```C++
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

	EasyCl::SourceCode sourceCode("example.cl");
	if(sourceCode.good() == false)
		exit(0);
	//cout << sourceCode.code << endl;

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
```
`example.cl`
```OpenCL
__kernel void neatStuff(__global int* target, __global int* source)
{
	int id = get_global_id(0);
	target[id] = source[id]+id;
}
```
If everthing is working normally. It should print out some info about your OpneCL device, than numbers from 0 to 29.<br>
Ex : <br>
```
selected device:
Name:		Intel(R) HD Graphics
OpenCL version:	OpenCL 1.2 
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 
```

##Appendix
###Supported gfx and SDK
The following are tested gfx and associated SDK. These suite should run our examples smoothly.

| gfx                                    | OpenCL     | SDK                                                         |
| -------------------------------------- | :--------: | ----------------------------------------------------------: |
| Intel i7-4650U CPU @ 1.70GHz (HD 5000) | OpenCL 1.2 | intel_code_builder_for_opencl_2015_ubuntu_5.0.0.43_x64.tgz  |

###Unsupported gfx and SDK
The following are tested gfx and associated SDK with known or unknown issues. These suite did not run our examples smoothly.

| gfx                                    | OpenCL     | SDK                                                         |
| -------------------------------------- | :--------: | ----------------------------------------------------------: |
| Intel i7-4650U CPU @ 1.70GHz (HD 5000) | OpenCL 1.2 | intel_code_builder_for_opencl_mss_2015_5.0.0.93_x64.tgz     |

[NDRange-Explain]: https://www.khronos.org/message_boards/showthread.php/6287-Beginner-question-understanding-NDRange
