#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

#include "detect_exoplanets.h"


DetectExoplanets::Exoplanet::Exoplanet() { };  // called if there is no planet identified


DetectExoplanets::Exoplanet::Exoplanet(float firstTransitDate, float flux, float period)
{
	isPlanet = true;
	this->firstTransitDate = firstTransitDate;
	this->flux = flux;
	this->period = period;
}

std::ostream& DetectExoplanets::operator<<(std::ostream& strm, const DetectExoplanets::Exoplanet& exoplanet) {
	return strm << "First observed transit date: " << exoplanet.firstTransitDate <<
				   "\nFirst observed flux: " << exoplanet.flux <<
				   "\nPeriod: " << exoplanet.period << " days\n";
}

/*
Filters through the data to find datapoints under
the value potentialPlanetThreshold
*/
std::unordered_map<std::string, std::vector<float>> DetectExoplanets::FindPlanet::findPlanetCandidates()
{
	if (flux.size() != date.size()) {
		std::cout << "Flux size is not equal to date size.";
		exit(2);
	}

	std::unordered_map<std::string, std::vector<float>> returnValue;

	for (int i = 0; i < flux.size(); i++) {
		if (flux[i] <= potentialPlanetThreshold) {
			returnValue["flux"].push_back(flux[i]);
			returnValue["date"].push_back(date[i]);
		}
	}

	return returnValue;
}

/*
Groups datapoints from the same transit into one datapoint with the peak
datapoint with the corresponding Julian date.
*/
std::unordered_map<std::string, std::vector<float>> DetectExoplanets::FindPlanet::groupDatapoints(
	std::unordered_map<std::string, std::vector<float>> data)
{
	std::unordered_map<std::string, std::vector<float>> returnValue;
	returnValue["flux"] = {};
	returnValue["date"] = {};

	float peakFlux = 2;
	float peakFluxDate = 0;
	float startDate = 0;

	for (int i = 0; i < data["flux"].size(); i++) {
		// if a new transit has occured
		if (data["date"][i] - startDate > maxTransitDurationDays) {
			// If the values are not the default values
			if (!(peakFlux == 2 && peakFluxDate == 0)) {
				returnValue["flux"].push_back(peakFlux);
				returnValue["date"].push_back(peakFluxDate);
			}

			peakFlux = data["flux"][i];
			peakFluxDate = data["date"][i];
			startDate = data["date"][i];
		}

		if (data["flux"][i] < peakFlux) {
			peakFlux = data["flux"][i];
			peakFluxDate = data["date"][i];
		}
	}

	// If the values are not the default values
	if (!(peakFlux == 2 && peakFluxDate == 0)) {
		returnValue["flux"].push_back(peakFlux);
		returnValue["date"].push_back(peakFluxDate);
	}

	return returnValue;
}

/*
Splits the grouped datapoints into different unordered maps by the
size of the planet. Used for checking if the period between the data
is the same, meaning that it is a planet.
*/
std::vector<std::unordered_map<std::string, std::vector<float>>> DetectExoplanets::FindPlanet::splitDatapoints(
	std::unordered_map<std::string, std::vector<float>> data)
{
	std::vector<std::unordered_map<std::string, std::vector<float>>> returnVector;

	for (int i = 0; i < data["flux"].size(); i++) {
		bool broken = false;

		for (int j = 0; j < returnVector.size(); j++) {
			if (abs(returnVector[j]["flux"][0] - data["flux"][i]) <= samePlanetSizeThreshold) {
				returnVector[j]["flux"].push_back(data["flux"][i]);
				returnVector[j]["date"].push_back(data["date"][i]);

				broken = true;
				break;
			}
		}

		if (broken) {
			continue;
		}

		std::unordered_map<std::string, std::vector<float>> newGroup;
		newGroup["flux"] = { data["flux"][i] };
		newGroup["date"] = { data["date"][i] };
		returnVector.push_back(newGroup);
	}

	return returnVector;
}

/*
Returns -1 if a planet is not found. Otherwise returns the Julian date of the first transit.

WARNING: This algorithm may give a false negative when two planets are in the same dataset.
		 This would happen if two planets are similar size, it wouldn't get
		 picked up by the grouping algorithm.
*/
DetectExoplanets::Exoplanet DetectExoplanets::FindPlanet::planetInData(std::unordered_map<std::string, std::vector<float>> data)
{
	if (data["flux"].size() < 3) {
		return Exoplanet{};  // not enough datapoints
	}

	for (int i = 0; i < data["date"].size() - 2; i++) {
		float transitTime1 = data["date"][i + 1] - data["date"][i];
		float transitTime2 = data["date"][i + 2] - data["date"][i + 1];

		if (abs(transitTime2 - transitTime1) < TTVRange) {
			return Exoplanet{ data["date"][i], data["flux"][i], (transitTime1 + transitTime2) / 2 };
		}
	}

	return Exoplanet{};
}

/*
This method is to provide a more sophisticated algorithm which counteracts the
warning in the planetInData method, at the cost of time.
*/
DetectExoplanets::Exoplanet DetectExoplanets::FindPlanet::planetInDataPrecise(std::unordered_map<std::string, std::vector<float>> data)
{
	if (data["date"].size() < 3) {
		return Exoplanet{};
	}

	for (int i = 0; i < data["date"].size(); i++) {
		for (int j = i + 1; j < data["date"].size(); j++) {
			float period = data["date"][j] - data["date"][i];

			float closest = data["date"][j];
			int closestIndex = j;
			float nextExpectedTransit = data["date"][j] + period;
			float lastDiff = abs(closest - nextExpectedTransit);

			// Find the datapoint closest to the next expected transit
			for (int k = j; k < data["date"].size(); k++) {

				if (abs(data["date"][k] - nextExpectedTransit) > lastDiff) {
					break;
				}

				lastDiff = abs(data["date"][k] - nextExpectedTransit);
				closestIndex = k;
				closest = data["date"][k];
			}

			if (abs(nextExpectedTransit - closest) < TTVRange) {
				return Exoplanet{data["date"][i], data["flux"][i], period};
			}
		}
	}

	return Exoplanet{};
}

/*
Calls other methods necessary to find planets
*/
std::vector<DetectExoplanets::Exoplanet> DetectExoplanets::FindPlanet::findPlanets(bool verbose)
{
	auto candidates = findPlanetCandidates();
	auto grouped = groupDatapoints(candidates);
	auto splitted = splitDatapoints(grouped);

	std::vector<Exoplanet> planets;

	for (auto& group : splitted) {
		Exoplanet planetInfo = planetInData(group);

		if (planetInfo.isPlanet) {
			planets.push_back(planetInfo);
		}
	}
	
	// Print additional info if verbose is enabled, mainly used for debugging
	if (verbose) {
		std::cout << "*** Candidates ***\n\n";

		for (int i = 0; i < candidates["flux"].size(); i++) {
			std::cout << candidates["flux"][i] << "," << candidates["date"][i] << "\n";
		}

		std::cout << "\n\n*** Grouped ***\n\n";

		for (int i = 0; i < grouped["flux"].size(); i++) {
			std::cout << grouped["flux"][i] << "," << grouped["date"][i] << "\n";
		}

		for (int i = 0; i < splitted.size(); i++) {
			std::cout << "\n\n*** SPLIT GROUP " << i << " ***\n\n";

			for (int j = 0; j < splitted[i]["flux"].size(); j++) {
				std::cout << splitted[i]["flux"][j] << "," << splitted[i]["date"][j] << "\n";
			}
		}

		std::cout << "\n\n*** Planet Fluxes *** \n\n";

		for (auto planet : planets) {
			std::cout << planet << "\n";
		}

		std::cout << "\n";
	}

	return planets;
}

/*
Uses a different algorithm to find more exoplanets at the expense of time
*/
std::vector<DetectExoplanets::Exoplanet> DetectExoplanets::FindPlanet::findPlanetsPrecise(bool verbose)
{
	auto candidates = findPlanetCandidates();
	auto grouped = groupDatapoints(candidates);
	auto splitted = splitDatapoints(grouped);

	std::vector<Exoplanet> planets;

	for (auto splitData : splitted) {
		Exoplanet planet = planetInDataPrecise(splitData);

		if (planet.isPlanet) {
			planets.push_back(planet);
		}
	}

	if (verbose) {
		std::cout << "*** Candidates ***\n\n";

		for (int i = 0; i < candidates["flux"].size(); i++) {
			std::cout << candidates["flux"][i] << "," << candidates["date"][i] << "\n";
		}

		std::cout << "\n\n*** Grouped ***\n\n";

		for (int i = 0; i < grouped["flux"].size(); i++) {
			std::cout << grouped["flux"][i] << "," << grouped["date"][i] << "\n";
		}

		for (int i = 0; i < splitted.size(); i++) {
			std::cout << "\n\n*** SPLIT GROUP " << i << " ***\n\n";

			for (int j = 0; j < splitted[i]["flux"].size(); j++) {
				std::cout << splitted[i]["flux"][j] << "," << splitted[i]["date"][j] << "\n";
			}
		}

		std::cout << "\n\n*** Planet Fluxes *** \n\n";

		for (auto planet : planets) {
			std::cout << planet << "\n";
		}

		std::cout << "\n";
	}

	return planets;
}

/*
data: The data from the observed star's brightness. It will have two key values of "flux" and "date".

planetThreshold: The minimum amount the star's brightness can dip under for it to be considered a potential
					datapoint with a planet inside of it.

sizeThreshold: The range that the star's appeared brightness can vary and be considered the same planet

maxTransitDurationDays: Used for grouping data. The amount of time, in days, that the transit can take.

TTVRange: The amount of variation the period of a planet can have, in days, to be considered the same planet.
*/
DetectExoplanets::FindPlanet::FindPlanet(std::unordered_map<std::string, std::vector<float>> data,
	float planetThreshold, float sizeThreshold, float maxTransitDurationDays,
	float TTVRange)
{
	this->flux = data["flux"];
	this->date = data["date"];

	potentialPlanetThreshold = planetThreshold;
	samePlanetSizeThreshold = sizeThreshold;
	this->maxTransitDurationDays = maxTransitDurationDays;
	this->TTVRange = TTVRange;
}