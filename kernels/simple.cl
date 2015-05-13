__kernel void neatStuff(__global int* target, __global int* source)
{
	int id = get_global_id(0);
	target[id] = source[id]+id;
}
