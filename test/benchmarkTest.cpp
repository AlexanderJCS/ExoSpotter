#include <iostream>
#include <chrono>
#include <cmath>

#include "../include/exoSpotter/exoSpotter.h"
#include "benchmarkTest.h"


Timer::Timer()
{
	start = std::chrono::high_resolution_clock::now();
}


double Timer::time()
{
	auto now = std::chrono::high_resolution_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::microseconds>(now - start);

	return (double) time.count() / 1000;
}


double average(double* arr, const unsigned int size)
{
	long double sum = 0;

	for (int i = 0; i < size; i++) {
		sum += arr[i];
	}

	return sum / size;
}


double stdDeviation(double* arr, const unsigned int size, const double mean)
{
	long double numerator = 0;

	for (int i = 0; i < size; i++) {
		double distFromMean = arr[i] - mean;
		numerator += distFromMean * distFromMean;
	}

	return std::sqrt(numerator / size);
}


void benchmark(ExoSpotter::FindPlanet planetFinder, const unsigned int runs)
{
	double* times = new double[runs];

	for (int i = 0; i < runs; i++) {
		Timer testCase;
		planetFinder.findPlanets();
		times[i] = testCase.time();
	}

	double averageTime = average(times, runs);
	double stdDev = stdDeviation(times, runs, averageTime);

	std::cout << "Average Time: " << averageTime << " ms\n";
	std::cout << "Standard Deviation: " << stdDev << " ms\n";

	delete[] times;
}
