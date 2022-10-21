#include <unordered_map>
#include <iostream>
#include <string>
#include <chrono>

#include "../include/exoplanetFinder/detectExoplanets.h"
#include "../include/exoplanetFinder/parseData.h"

using namespace std::chrono;


int main()
{
	ExoplanetFinder::FindPlanet planetFinder{ readData("test/wasp-126_data.csv"), 0.9999, 0.003, 1.5, 0.2, 2 };
	
	auto start = high_resolution_clock::now();
	auto planets = planetFinder.findPlanets(false);
	auto stop = high_resolution_clock::now();

	std::cout << "*** FAST ALGORITHM ***\n";

	for (int i = 0; i < planets.size(); i++) {
		std::cout << "Planet " << i + 1 << ":\n" << planets[i] << "\n";
	}

	auto duration = duration_cast<milliseconds>(stop - start);
	std::cout << "\nCompleted in " << duration.count() << " ms\n";

	start = high_resolution_clock::now();
	planets = planetFinder.findPlanetsPrecise(true);
	stop = high_resolution_clock::now();

	std::cout << "\n*** PRECISE ALGORITHM ***\n";

	for (int i = 0; i < planets.size(); i++) {
		std::cout << "Planet " << i + 1 << ":\n" << planets[i] << "\n";
	}

	duration = duration_cast<milliseconds>(stop - start);
	std::cout << "\nCompleted in " << duration.count() << " ms\n";
}