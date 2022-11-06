

class Timer
{
	std::chrono::high_resolution_clock::time_point start;

public:
	Timer();
	double time();
};


double average(double* arr, const unsigned int size);
double stdDeviation(double* arr, const unsigned int size, const double mean);

void benchmark(ExoSpotter::FindPlanet planetFinder, const unsigned int runs);
