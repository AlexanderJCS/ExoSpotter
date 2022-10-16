#include <unordered_map>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>

#include "../include/exoplanetFinder/detect_exoplanets.h"
#include "../include/exoplanetFinder/parseData.h"

using namespace std::chrono;


int main()
{
	DetectExoplanets::FindPlanet planetFinder{ readData("test/data.csv"), 0.99995, 0.001, 1.5, 0.5 };
	
	auto start = high_resolution_clock::now();
	auto planets = planetFinder.findPlanets(false);
	auto stop = high_resolution_clock::now();

	std::cout << "*** FAST ALGORITHM ***\n";

	for (const auto& planet : planets) {
		std::cout << "Planet identified with flux: " << planet << "\n";
	}

	auto duration = duration_cast<milliseconds>(stop - start);
	std::cout << "\nCompleted in " << duration.count() << " ms\n";

	start = high_resolution_clock::now();
	planets = planetFinder.findPlanetsPrecise(false);
	stop = high_resolution_clock::now();

	std::cout << "\n *** PRECISE ALGORITHM ***\n";

	for (auto& planet : planets) {
		std::cout << planet << "\n";
	}

	duration = duration_cast<milliseconds>(stop - start);
	std::cout << "\nCompleted in " << duration.count() << " ms\n";
}