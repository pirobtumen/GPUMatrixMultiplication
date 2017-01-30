#include <chrono>

class Clock {
private:
	// Hold the start time
	std::chrono::high_resolution_clock::time_point start_time;

public:
	Clock();

	void start();
	double now();
};
