//
//	Matrix Multiplication Benchmark 
//
//	Two matrix multiplication. Size: 3000*3000.
//
//	- OpenCL
//
//	Alberto Sola - 2016
// ----------------------------------------------------------------------------


#include "stdafx.h"
#include "clock.h"


// ----------------------------------------------------------------------------


template <typename T>
void random_init(T & matrix, unsigned int size){
	/*
		Initialize a vector with random numbers.
	*/
	const int DIV = 200;
	
	for (unsigned int i = 0; i < size; i++)
		matrix[i] = (float)((rand() % 100) + 1) / ((rand() % DIV) + 1);
}


// ----------------------------------------------------------------------------

double matrix_multiplication(cl::Platform & platform, cl::Device & device) {
	/*
		Matrix multiplicatioin using each device found with OpenCL.
	*/
	const unsigned int ROW_SIZE = 3000;
		
	Clock timer;
	double time_spent;

	// TODO: pass as a parameter.
	int matrix_size = ROW_SIZE * ROW_SIZE;
	unsigned int matrix_bytes_size = sizeof(float) * matrix_size;
	float * A = new float[matrix_size];
	float * B = new float[matrix_size];
	float * C = new float[matrix_size];

	// Initialize seed
	srand(1234);

	// Initalize matrix
	random_init(A, matrix_size);
	random_init(B, matrix_size);


	// Initialize OpenCL
	cl::Context context({ device });
	cl::Program::Sources sources;

	// Load kernel: Matrix multiplication
	std::ifstream kernel_file("matrixmult.cl");
	std::string kernel_code((std::istreambuf_iterator<char>(kernel_file)), std::istreambuf_iterator<char>());

	sources.push_back({ kernel_code.c_str(),kernel_code.length() });

	cl::Program program(context, sources);
	if (program.build({ device }) != CL_SUCCESS) {
		std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << "\n";
		exit(1);
	}

	// Get the current time
	timer.start();

	// Create buffers on the device
	cl::Buffer buffer_A(context, CL_MEM_READ_ONLY, matrix_bytes_size);
	cl::Buffer buffer_B(context, CL_MEM_READ_ONLY, matrix_bytes_size);
	cl::Buffer buffer_C(context, CL_MEM_READ_WRITE, matrix_bytes_size);

	// Create queue to which we will push commands for the device.
	cl::CommandQueue queue(context, device);

	// Write arrays A and B to the device
	queue.enqueueWriteBuffer(buffer_A, CL_TRUE, 0, matrix_bytes_size, A);
	queue.enqueueWriteBuffer(buffer_B, CL_TRUE, 0, matrix_bytes_size, B);


	// Run the kernel
	cl::Kernel matrix_mul_kernel(program, "matrix_multiplication");
	matrix_mul_kernel.setArg(0, ROW_SIZE);
	matrix_mul_kernel.setArg(1, buffer_A);
	matrix_mul_kernel.setArg(2, buffer_B);
	matrix_mul_kernel.setArg(3, buffer_C);

	int err = queue.enqueueNDRangeKernel(matrix_mul_kernel, cl::NullRange, cl::NDRange(ROW_SIZE,ROW_SIZE), cl::NullRange);
	if (err != CL_SUCCESS)
		std::cout << "Err " << err << std::endl;

	// Sync
	queue.finish();
	//queue.enqueueBarrierWithWaitList();

	// Read result C from the device to array C
	queue.enqueueReadBuffer(buffer_C, CL_TRUE, 0, matrix_bytes_size, C);

	// Get the time
	time_spent = timer.now();
	
	/*
	std::cout << "A" << std::endl;

	for (unsigned int i = 0; i < ROW_SIZE; i++) {
		for (unsigned int j = 0; j < ROW_SIZE; j++)
			std::cout << A[i*ROW_SIZE + j] << " ";
		std::cout << std::endl;
	}

	std::cout << "B" << std::endl;

	for (unsigned int i = 0; i < ROW_SIZE; i++) {
		for (unsigned int j = 0; j < ROW_SIZE; j++)
			std::cout << B[i*ROW_SIZE + j] << " ";
		std::cout << std::endl;
	}

	std::cout << "C" << std::endl;

	for (unsigned int i = 0; i < ROW_SIZE; i++) {
		for (unsigned int j = 0; j < ROW_SIZE; j++)
			std::cout << C[i*ROW_SIZE + j] << " ";
		std::cout << std::endl;
	}
	*/

	delete[] A;
	delete[] B;
	delete[] C;

	return time_spent;
}

// ----------------------------------------------------------------------------

int main(){
	const std::string VERSION = "V0.1.0";
	double time_spent;

	// Show benchmark info
	std::cout << "OpenCL Matrix Multiplication Benchmark - " << VERSION << std::endl;
	std::cout << "----------------------------------------------------------" << std::endl;

	std::cout << std::endl;
	std::cout << "Multiplication of two matrix. The size is like a 4K image." << std::endl;
	std::cout << std::endl;

	// Get all platforms (drivers)
	std::vector<cl::Platform> all_platforms;
	cl::Platform::get(&all_platforms);
	if (all_platforms.size() == 0) {
		std::cout << " No platforms found. Check OpenCL installation!\n";
		exit(1);
	}

	// Run the test on each device
	for (auto & platform : all_platforms) {
		std::cout << "- Detected platform: " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
		std::vector<cl::Device> all_devices;
		platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);

		if (all_devices.size() == 0) {
			std::cout << "No devices found." << std::endl;
		}
		else {
			for (auto & device : all_devices) {
				std::cout << "\t- " << device.getInfo<CL_DEVICE_NAME>() << "\t- Time: ";
				time_spent = matrix_multiplication(platform, device);
				std::cout << time_spent << "s" << std::endl;
			}
		}

		std::cout << std::endl;
	}

	std::cout << "Press enter to continue...";
	getchar();

    return 0;
}