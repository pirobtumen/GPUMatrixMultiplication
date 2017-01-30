#include "stdafx.h"
#include "clock.h"


Clock::Clock(){
	start();
}

void Clock::start() {
	/*
		Set the 'start' time to the current time.
	*/
	start_time = std::chrono::high_resolution_clock::now();
}

double Clock::now() {
	/*
		Return a double with the time between start and the moment when you call now().
	*/
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time = std::chrono::duration_cast<std::chrono::duration<double>>(end - start_time);

	return time.count();
}
