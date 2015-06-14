/*This is workound of the issue that OpenCL apprenly have the ability of using
double pointers, but not avliable on kernel arguments.
How this works is basiclly create a data type which stores pointers with no
issue. size_t is it. Than, simplly assign the pointer to yout storage. Then just
convert to the original pointer type when you need it. Isn't it brillant?*/
typedef size_t ECLPtr;

//A function to copy pointer to a array(storage)
__kernel void upload(__global ECLPtr* storage, __global void* data, __global int* index)
{
	storage[*index] = (ECLPtr)data;
	/*__global int* ptr = (__global int*) storage[*index];
	for(int i=0;i<0xffff;i++)
		if(ptr[i] != 0)
			printf("%d ",ptr[i]);*/
}

__kernel void copy(__global int* target, __global ECLPtr* source)
{
	int id = get_global_id(0);
	__global int* ptr = (__global int*) source[0];
	target[id] = ptr[id];
	if(ptr[id] != 0)
		printf("%d ",ptr[id]);
}
