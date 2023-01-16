#include "Timer.h"

using namespace std;
using namespace std::chrono;

Timer::Timer()
{
	reset();
}

void Timer::reset()
{
	total = 0.0;
	running = false;
}

void Timer::start()
{
	if (running) return;
	start_point = system_clock::now();
	running = true;
}

double Timer::lap()
{
	if (!running) return numeric_limits<double>::infinity();
	auto now = system_clock::now();
	double duration = duration_cast<microseconds>(now - start_point).count() / 1000000.0;
	return total + duration;
}

double Timer::stop()
{
	total = lap();
	running = false;
	return total;
}
