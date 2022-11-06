#include <iostream>

#include "../include/exoSpotter/exoSpotter.h"
#include "findPlanetTest.h"


void findPlanetTest(ExoSpotter::FindPlanet planetFinder)
{
	auto planets = planetFinder.findPlanets(false);

	for (int i = 0; i < planets.size(); i++) {
		std::cout << "Planet " << i + 1 << ":\n" << planets[i];
		std::cout << "Radius percent: " << planets[i].findRadiusRatio() * 100 << "%\n";
		std::cout << "Confidence: " << planets[i].confidence() * 100 << "%\n\n";

		std::cout << "\nDatapoints:\n";

		for (int j = 0; j < planets[i].datapoints().size(); j++) {
			std::cout << planets[i].datapoints().flux()[j] << ", " << planets[i].datapoints().date()[j] << "\n";
		}

		std::cout << "\n";
	}
}
