#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

#include "detect_exoplanets.h"

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
float DetectExoplanets::FindPlanet::planetInData(std::vector<float> data)
{
	if (data.size() < 3) {
		return -1;  // not enough datapoints
	}

	for (int i = 0; i < data.size() - 2; i++) {
		float gap1 = data[i + 1] - data[i];
		float gap2 = data[i + 2] - data[i + 1];

		if (abs(gap2 - gap1) < TTVRange) {
			return data[i];
		}
	}

	return -1;
}

/*
Returns -1 if a planet is not found. Otherwise returns the date
of the flux of the of the planet found.

This method is to provide a more sophisticated algorithm which counteracts the
warning in the planetInData method, at the cost of time.
*/
float DetectExoplanets::FindPlanet::planetInDataPrecise(std::vector<float> data)
{
	std::vector<float> returnVector;  // periods of the planets

	for (int i = 0; i < data.size(); i++) {
		for (int j = i + 1; j < data.size(); j++) {
			float period = data[j] - data[i];

			float closest = data[j];
			int closestIndex = j;
			float nextExpectedTransit = data[j] + period;
			float lastDiff = abs(closest - nextExpectedTransit);

			// Find the datapoint closest to the next expected transit
			for (int k = j; k < data.size(); k++) {

				if (abs(data[k] - nextExpectedTransit) > lastDiff) {
					break;
				}

				lastDiff = abs(data[k] - nextExpectedTransit);
				closestIndex = k;
				closest = data[k];
			}

			if (abs(nextExpectedTransit - closest) < TTVRange) {
				return data[i];
			}
		}
	}

	return -1;
}

/*
Uses binary search to find the date index
*/
int DetectExoplanets::FindPlanet::findDateIndex(std::vector<float> data, float target)
{
	auto lowerBoundIt = std::lower_bound(data.begin(), data.end(), target);
	
	if (lowerBoundIt == data.end() || *lowerBoundIt != target) {
		return -1;
	}
	
	else {
		return std::distance(data.begin(), lowerBoundIt);
	}
}

/*
Calls other methods necessary to find planets
*/
std::vector<float> DetectExoplanets::FindPlanet::findPlanets(bool verbose)
{
	auto candidates = findPlanetCandidates();
	auto grouped = groupDatapoints(candidates);
	auto splitted = splitDatapoints(grouped);

	std::vector<float> planetFluxes;

	for (auto& group : splitted) {
		float date = planetInData(group["date"]);

		if (date != -1) {
			int index = findDateIndex(group["date"], date);

			// If the item is found
			if (index != -1) {
				planetFluxes.push_back(group["flux"][index]);
			}
			
			// Else, resort by putting the first item in the group
			else {
				planetFluxes.push_back(group["flux"][0]);
			}
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

		for (auto flux : planetFluxes) {
			std::cout << flux << "\n";
		}

		std::cout << "\n";
	}

	return planetFluxes;
}

/*
Uses a different algorithm to find more exoplanets at the expense of time
*/
std::vector<float> DetectExoplanets::FindPlanet::findPlanetsPrecise(bool verbose)
{
	auto candidates = findPlanetCandidates();
	auto grouped = groupDatapoints(candidates);
	auto splitted = splitDatapoints(grouped);

	std::vector<float> planetFluxes;

	for (auto splitData : splitted) {
		float planetFlux = planetInDataPrecise(splitData["date"]);

		if (planetFlux != -1) {
			planetFluxes.push_back(splitData["flux"][findDateIndex(splitData["date"], planetFlux)]);
		}
	}

	return planetFluxes;
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