#include <unordered_map>
#include <iostream>
#include <string>
#include <chrono>

#include "../include/exoSpotter/exoSpotter.h"
#include "../include/exoSpotter/parseData.h"

using namespace std::chrono;


int main()
{
	ExoSpotter::FindPlanet planetFinder{ 
		readData("test/wasp-126_data.csv"), 0.9999, 0.003, 1.0, 0.1, 2 
	};

	auto start = high_resolution_clock::now();
	auto planets = planetFinder.findPlanets(false);
	auto stop = high_resolution_clock::now();

	std::cout << "*** FAST ALGORITHM ***\n";

	for (int i = 0; i < planets.size(); i++) {
		std::cout << "Planet " << i + 1 << ":\n" << planets[i];
		std::cout << "Radius percent: " << planets[i].findRadiusRatio() * 100 << "%\n\n";
	}

	auto duration = duration_cast<microseconds>(stop - start);
	std::cout << "\nCompleted in " << duration.count() << " microseconds\n";

	start = high_resolution_clock::now();
	planets = planetFinder.findPlanetsPrecise(false);
	stop = high_resolution_clock::now();

	std::cout << "\n*** PRECISE ALGORITHM ***\n";

	for (int i = 0; i < planets.size(); i++) {
		std::cout << "Planet " << i + 1 << ":\n" << planets[i];
		std::cout << "Radius percent: " << planets[i].findRadiusRatio() * 100 << "%\n\n";
	}

	duration = duration_cast<microseconds>(stop - start);
	std::cout << "\nCompleted in " << duration.count() << " microseconds\n";
}