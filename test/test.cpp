#include <unordered_map>
#include <iostream>
#include <string>
#include <chrono>

#include "../include/exoSpotter/exoSpotter.h"
#include "../include/exoSpotter/parseData.h"

#include "findPlanetTest.h"
#include "benchmarkTest.h"


using namespace std::chrono;

const unsigned int timedTestRuns = 100;


int main()
{
	ExoSpotter::FindPlanet planetFinder{
		readData("test/wasp-126_data.csv"), 0.9999, 0.005, 1.5, 0.1, 0.5
	};

	std::cout << "*** FIND PLANETS TEST ***\n";
	findPlanetTest(planetFinder);

	std::cout << "*** BENCHMARKING ***\n";
	std::cout << "Performing benchmark test. Please wait.\n";
	benchmark(planetFinder, timedTestRuns);
}