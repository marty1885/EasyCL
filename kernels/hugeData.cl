__kernel void porcess(__global char* buffer)
{
	int id = get_global_id(0);
	buffer[id] = 1;
}
