__kernel void reallySmallRSACrack(__global int* num, __global int* reasult, __global int* communicateBuffer)
{
	int targetNum = *num;
	int squareRoot = sqrt(convert_float(targetNum));
	int size = get_global_size(0);
	int id = get_global_id(0);
	int calcNum = squareRoot - id;

	//calcNum > 1 insures no process will find the solution calcNum=1 which is something we don't want.
	for(int i=id;i<squareRoot;calcNum > 1)
	{
		if(*communicateBuffer == 1)
			break;
		if(targetNum % calcNum == 0 && calcNum != 1)
		{
			*communicateBuffer = 1;
			reasult[0] = calcNum;
			reasult[1] = targetNum / calcNum;
		}
		calcNum -= size;
		i += size;
	}
}
