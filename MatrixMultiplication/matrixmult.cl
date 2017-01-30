

__kernel void matrix_multiplication(
	unsigned int SIZE,
	__global const float* A,
	__global const float *B,
	__global float *C)
{
	int row = get_global_id(0);
	int col = get_global_id(1);

	float sum_tmp = 0;

	//printf("%d\n",row);
	//printf("%d\n", col);

	for (int k = 0; k<SIZE; ++k)
		sum_tmp += A[row*SIZE + k] * B[k*SIZE + col];

	C[row*SIZE + col] = sum_tmp;
}