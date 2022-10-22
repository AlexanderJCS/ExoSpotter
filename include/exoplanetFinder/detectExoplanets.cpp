#include <stdexcept>
#include <optional>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

#include "detectExoplanets.h"


ExoplanetFinder::Lightcurve::Lightcurve() { }


ExoplanetFinder::Lightcurve::Lightcurve(std::vector<float> flux, std::vector<float> date)
{
	if (flux.size() != date.size()) {
		throw std::invalid_argument("Lightcurve vector sizes must be the same");
	}

	this->flux = flux;
	this->date = date;
}


ExoplanetFinder::Lightcurve ExoplanetFinder::Lightcurve::slice(int beginIndex, int endIndex)
{
	if (beginIndex > endIndex) {
		return Lightcurve{};
	}

	;
	std::vector<float> cutDate;

	auto fluxBegin = this->flux.begin() + beginIndex;
	auto fluxEnd = this->flux.begin() + endIndex + 1;

	auto dateBegin = this->date.begin() + beginIndex;
	auto dateEnd = this->date.begin() + endIndex + 1;

	std::vector<float> slicedFlux(fluxBegin, fluxEnd);
	std::vector<float> slicedDate(dateBegin, dateEnd);

	return Lightcurve{ slicedFlux, slicedDate };
}


ExoplanetFinder::Exoplanet::Exoplanet(Lightcurve planetDatapoints)
{
	this->planetDatapoints = planetDatapoints;

	float fluxAverage = 0;
	float periodAverage = 0;

	for (int i = 0; i < planetDatapoints.flux.size(); i++) {
		fluxAverage += planetDatapoints.flux[i];
		
		if (i != 0) {
			periodAverage += planetDatapoints.date[i] - planetDatapoints.date[i - 1];
		}
	}

	fluxAverage /= planetDatapoints.flux.size();

	if (planetDatapoints.date.size() != 0) {
		periodAverage /= planetDatapoints.date.size() - 1;
	}

	this->averagePeriod = periodAverage;
	this->averageFlux = fluxAverage;
}


/*
Finds the planet's semi-major axis (orbital radius) given the star's solar masses.
*/
float ExoplanetFinder::Exoplanet::findSemiMajAxis(float starSolarMasses)
{
	// T^2 / r^3 = 1 / M (solar masses)
	// r = cube root(M * T^2)
	return std::cbrt(starSolarMasses * (averagePeriod * averagePeriod));
}


/*
Finds the ratio of the planet radius / host star radius.
Returns -1 if the ratio cannot be found (usually due to there being no flux).
*/
float ExoplanetFinder::Exoplanet::findRadiusRatio()
{
	if (averageFlux > 0) {
		return std::sqrt(1 - averageFlux);
	}
	
	return -1;
}


std::ostream& ExoplanetFinder::operator<<(std::ostream& strm, const ExoplanetFinder::Exoplanet& exoplanet) {
	return strm << "First observed transit date: " << exoplanet.planetDatapoints.date[0] <<
				   "\nFirst observed flux: " << exoplanet.planetDatapoints.flux[0] <<
				   "\nPeriod: " << exoplanet.averagePeriod << " days\n";
}


/*
Filters through the data to find datapoints under
the value potentialPlanetThreshold
*/
ExoplanetFinder::Lightcurve ExoplanetFinder::FindPlanet::findPlanetCandidates()
{
	if (rawData.flux.size() != rawData.date.size()) {
		std::cout << "Flux size is not equal to date size.";
		exit(2);
	}

	Lightcurve planetCandidates;

	for (int i = 0; i < rawData.flux.size(); i++) {
		if (rawData.flux[i] <= potentialPlanetThreshold) {
			planetCandidates.flux.push_back(rawData.flux[i]);
			planetCandidates.date.push_back(rawData.date[i]);
		}
	}

	return planetCandidates;
}


/*
Groups datapoints from the same transit into one datapoint with the peak
datapoint with the corresponding Julian date.
*/
ExoplanetFinder::Lightcurve ExoplanetFinder::FindPlanet::groupDatapoints(Lightcurve data)
{
	Lightcurve groupedData;

	float peakFlux = 2;
	float peakFluxDate = 0;
	float startDate = 0;

	for (int i = 0; i < data.flux.size(); i++) {
		// if a new transit has occured
		if (data.date[i] - startDate > maxTransitDurationDays) {
			// If the values are not the default values
			if (!(peakFlux == 2 && peakFluxDate == 0)) {
				groupedData.flux.push_back(peakFlux);
				groupedData.date.push_back(peakFluxDate);
			}

			peakFlux = data.flux[i];
			peakFluxDate = data.date[i];
			startDate = data.date[i];
		}

		if (data.flux[i] < peakFlux) {
			peakFlux = data.flux[i];
			peakFluxDate = data.date[i];
		}
	}

	// If the values are not the default values
	if (!(peakFlux == 2 && peakFluxDate == 0)) {
		groupedData.flux.push_back(peakFlux);
		groupedData.date.push_back(peakFluxDate);
	}

	return groupedData;
}


/*
Splits the grouped datapoints into different unordered maps by the
size of the planet. Used for checking if the period between the data
is the same, meaning that it is a planet.
*/
std::vector<ExoplanetFinder::Lightcurve> ExoplanetFinder::FindPlanet::splitDatapoints(Lightcurve data)
{
	std::vector<Lightcurve> splitData;

	for (int i = 0; i < data.flux.size(); i++) {
		bool broken = false;

		for (int j = 0; j < splitData.size(); j++) {
			if (abs(splitData[j].flux[0] - data.flux[i]) <= samePlanetSizeThreshold) {
				splitData[j].flux.push_back(data.flux[i]);
				splitData[j].date.push_back(data.date[i]);

				broken = true;
				break;
			}
		}

		if (broken) {
			continue;
		}

		Lightcurve newGroup;
		newGroup.flux = { data.flux[i] };
		newGroup.date = { data.date[i] };
		splitData.push_back(newGroup);
	}

	return splitData;
}


/*
Returns -1 if a planet is not found. Otherwise returns the Julian date of the first transit.

WARNING: This algorithm may give a false negative when two planets are in the same dataset.
		 This would happen if two planets are similar size, it wouldn't get
		 picked up by the grouping algorithm.
*/
std::optional<ExoplanetFinder::Exoplanet> ExoplanetFinder::FindPlanet::planetInData(Lightcurve data)
{
	if (data.flux.size() < 3) {
		return { };  // not enough datapoints
	}

	for (int i = 0; i < data.date.size() - 2; i++) {
		float transitTime1 = data.date[i + 1] - data.date[i];
		float transitTime2 = data.date[i + 2] - data.date[i + 1];

		if (abs(transitTime2 - transitTime1) < TTVRange) {
			return Exoplanet{ data.slice(i, i + 2)};
		}
	}

	return {};
}


/*
This method is to provide a more sophisticated algorithm which counteracts the
warning in the planetInData method, at the cost of time.
*/
std::vector<ExoplanetFinder::Exoplanet> ExoplanetFinder::FindPlanet::planetInDataPrecise(Lightcurve data)
{
	if (data.date.size() < 3) {
		return {};
	}

	std::vector<Exoplanet> detectedExoplanets;

	for (int i = 0; i < data.date.size(); i++) {
		for (int j = i + 1; j < data.date.size(); j++) {
			float period = data.date[j] - data.date[i];

			float closest = data.date[j];
			int closestIndex = 0;
			float nextExpectedTransit = data.date[j] + period;
			float lastDiff = abs(closest - nextExpectedTransit);

			// Find the datapoint closest to the next expected transit
			for (int k = j; k < data.date.size(); k++) {

				if (abs(data.date[k] - nextExpectedTransit) > lastDiff) {
					break;
				}

				lastDiff = abs(data.date[k] - nextExpectedTransit);
				closest = data.date[k];
				closestIndex = k;
			}

			Exoplanet candidate = Exoplanet{ 
				Lightcurve{ 
					{data.flux[i], data.flux[j], data.flux[closestIndex]}, 
					{data.date[i], data.date[j], data.date[closestIndex]}
				}
			};

			// if the dist between the next expected transit and the closest acutal transit is
			// less than the TTVRange, then it is a possible planet.

			// If more than allowedMissedTransits were missed, exclude this planet. This is to
			// prevent noise data from being considered a real planet.
			if (fabs(nextExpectedTransit - closest) < TTVRange && 
				(data.date[i] - rawData.date[0]) / candidate.averagePeriod < allowedMissedTransits) {
				bool broken = false;

				// if the diff between the next expected transit and the closest acutal transit is
			// less than the TTVRange, then it is a possible planet.

			// If more than allowedMissedTransits were missed, exclude this planet. This is to
			// prevent noise data from being considered a real planet.
				if (abs(nextExpectedTransit - closest) < TTVRange &&
					(data.date[i] - data.date[0]) / candidate.averagePeriod < allowedMissedTransits) {
					bool broken = false;

					for (Exoplanet exoplanet : detectedExoplanets) {
						Exoplanet& greatest = exoplanet;
						Exoplanet& smallest = candidate;

						if (candidate.averagePeriod > exoplanet.averagePeriod) {
							greatest = candidate;
							smallest = exoplanet;
						}

						if (abs(greatest.averagePeriod - smallest.averagePeriod * round(greatest.averagePeriod / smallest.averagePeriod)) < TTVRange) {
							broken = true;
							break;
						}
					}

					if (!broken) {
						detectedExoplanets.push_back(candidate);
						break;
					}
				}
			}
		}
	}

	return detectedExoplanets;
}

void ExoplanetFinder::FindPlanet::printVerbose(
	Lightcurve candidates, Lightcurve grouped, std::vector<Lightcurve> splitted, std::vector<Exoplanet> planets)
{
	std::cout << "*** Candidates ***\n\n";

	for (int i = 0; i < candidates.flux.size(); i++) {
		std::cout << candidates.flux[i] << "," << candidates.date[i] << "\n";
	}

	std::cout << "\n\n*** Grouped ***\n\n";

	for (int i = 0; i < grouped.flux.size(); i++) {
		std::cout << grouped.flux[i] << "," << grouped.date[i] << "\n";
	}

	for (int i = 0; i < splitted.size(); i++) {
		std::cout << "\n\n*** SPLIT GROUP " << i << " ***\n\n";

		for (int j = 0; j < splitted[i].flux.size(); j++) {
			std::cout << splitted[i].flux[j] << "," << splitted[i].date[j] << "\n";
		}
	}

	std::cout << "\n\n*** Planet Fluxes *** \n\n";

	for (auto planet : planets) {
		std::cout << planet << "\n";
	}

	std::cout << "\n";
}


/*
Calls other methods necessary to find planets
*/
std::vector<ExoplanetFinder::Exoplanet> ExoplanetFinder::FindPlanet::findPlanets(bool verbose)
{
	auto candidates = findPlanetCandidates();
	auto grouped = groupDatapoints(candidates);
	auto splitted = splitDatapoints(grouped);

	std::vector<Exoplanet> planets;

	for (auto& group : splitted) {
		std::optional<Exoplanet> planetInfo = planetInData(group);

		if (planetInfo) {
			planets.push_back(planetInfo.value());
		}
	}
	
	// Print additional info if verbose is enabled, mainly used for debugging
	if (verbose) {
		printVerbose(candidates, grouped, splitted, planets);
	}

	return planets;
}


/*
Uses a different algorithm to find more exoplanets at the expense of time
*/
std::vector<ExoplanetFinder::Exoplanet> ExoplanetFinder::FindPlanet::findPlanetsPrecise(bool verbose)
{
	auto candidates = findPlanetCandidates();
	auto grouped = groupDatapoints(candidates);
	auto splitted = splitDatapoints(grouped);
	
	std::vector<Exoplanet> planets;

	for (auto splitData : splitted) {
		std::vector<Exoplanet> exoplanets = planetInDataPrecise(splitData);

		planets.insert(planets.end(), exoplanets.begin(), exoplanets.end());
	}

	if (verbose) {
		printVerbose(candidates, grouped, splitted, planets);
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
ExoplanetFinder::FindPlanet::FindPlanet(Lightcurve data, float planetThreshold, 
	float sizeThreshold, float maxTransitDurationDays, float TTVRange, int allowedMissedTransits)
{
	this->rawData.flux = data.flux;
	this->rawData.date = data.date;

	potentialPlanetThreshold = planetThreshold;
	samePlanetSizeThreshold = sizeThreshold;
	this->maxTransitDurationDays = maxTransitDurationDays;
	this->TTVRange = TTVRange;
	this->allowedMissedTransits = allowedMissedTransits;
}