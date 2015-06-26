# EasyCL
EasyCL - a library let you access the power of OpenCL easily. <br>
NOTE : EasyCL does not support images now. But is planned in the near future.<br>
NOTE : There is currently no plain on supporting OpenGL/Direct3D data access.<br>

#What it does
Basiclly EasyCL is a library that enables you to build and lunch a OpenCL program in just a few lines of code.<br><br>
OpenCL or Open Compute Language is the first open, royalty-free standard for cross-platform, parallel programming of modern processors found in personal computers, servers and handheld/embedded devices. OpenCL greatly improves speed and responsiveness for a wide spectrum of applications in numerous market categories from gaming and entertainment to scientific and medical software.<br>
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

#What it does NOT
1.There is currently no plain on supporting OpenGL/Direct3D data access.<br>
2.No guarantee to be able to be compiled on MSVC/Intel CC. If anyone needs it on those compilers. Send me a mail or post a issue. I'll do my best to help you.<br>
<br>
#Show me some code
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
By changine ```CL_DEVICE_TYPE_GPU``` into ```CL_DEVICE_TYPE_CPU``` or ```CL_DEVICE_TYPE_ACCLERAOR```, it will return  devices in dirrerent category.<br>
You can even do something like ```CL_DEVICE_GPU|CL_DEVICE_CPU``` to search in muiltple categories.<br>

####Loading some source code.
```C++
    EasyCl::SourceCode sourceCode("someOpenCLCode.cl");
```
It's just easy as that<br>
There is also ```bool EasyCl::SourceCode::good()``` that tells you weather the loading is success.

####Building program
In OpenCL, after loading your source code. What you wana to do next is to build a program.(compile OpenCL code)<br>
```C++
    EasyCl::Software program(device, sourceCode);
    program.build();
```
```int EasyCL::Software::build(std::string options)``` returns error code. If it returns ```CL_SUCCESS``` than the build is success.<br>
```build()``` also take a "option" argument. This is the OpenCL compilation argument. "option" will be driectly passed to OpenCL. EasyCL does not process/use it.<br>
To add options to the building process. just do ```program.build(<args here>);```. Such as ```program.build("-cl-denorms-are-zero -DOCL_KERNEL");```<br>
<br>
----To be continue----
