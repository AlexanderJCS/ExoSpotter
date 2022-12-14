#include <stdexcept>
#include <optional>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

#include "exoSpotter.h"


ExoSpotter::Lightcurve::Lightcurve(std::vector<float> flux, std::vector<float> date) : m_flux(flux), m_date(date)
{
	if (flux.size() != date.size()) {
		throw std::invalid_argument("Lightcurve vector sizes must be the same");
	}
}


ExoSpotter::Lightcurve ExoSpotter::Lightcurve::slice(int beginIndex, int endIndex)
{
	if (beginIndex > endIndex) {
		return Lightcurve{};
	}

	std::vector<float> cutDate;

	auto fluxBegin = this->m_flux.begin() + beginIndex;
	auto fluxEnd = this->m_flux.begin() + endIndex + 1;

	auto dateBegin = this->m_date.begin() + beginIndex;
	auto dateEnd = this->m_date.begin() + endIndex + 1;

	std::vector<float> slicedFlux(fluxBegin, fluxEnd);
	std::vector<float> slicedDate(dateBegin, dateEnd);

	return Lightcurve{ slicedFlux, slicedDate };
}


void ExoSpotter::Lightcurve::addPair(float flux, float date)
{
	m_flux.push_back(flux);
	m_date.push_back(date);
}


void ExoSpotter::Lightcurve::addPair(std::vector<float> flux, std::vector<float> date)
{
	if (flux.size() != date.size()) {
		throw std::invalid_argument("Flux size must be the same as date size when adding a pair.");
	}

	m_flux.insert(m_flux.end(), flux.begin(), flux.end());
	m_date.insert(m_date.end(), date.begin(), date.end());
}


size_t ExoSpotter::Lightcurve::size()
{
	return m_flux.size();
}


ExoSpotter::Exoplanet::Exoplanet(Lightcurve planetDatapoints, float confidence)
{
	this->m_datapoints = planetDatapoints;
	this->m_confidence = confidence;

	float fluxAverage = 0;
	float periodAverage = 0;

	for (int i = 0; i < planetDatapoints.size(); i++) {
		fluxAverage += planetDatapoints.flux()[i];

		if (i != 0) {
			periodAverage += planetDatapoints.date()[i] - planetDatapoints.date()[i - 1];
		}
	}

	fluxAverage /= planetDatapoints.size();

	if (planetDatapoints.size() != 0) {
		periodAverage /= planetDatapoints.size() - 1;
	}

	this->m_averagePeriod = periodAverage;
	this->m_averageFlux = fluxAverage;
}


/*
Finds the planet's semi-major axis (orbital radius) given the star's solar masses.
*/
float ExoSpotter::Exoplanet::findSemiMajAxis(float starSolarMasses)
{
	// T^2 / r^3 = 1 / M (solar masses)
	// r = cube root(M * T^2)
	return std::cbrt(starSolarMasses * (m_averagePeriod * m_averagePeriod));
}


/*
Finds the ratio of the planet radius / host star radius.
Returns -1 if the ratio cannot be found (usually due to there being no flux).
*/
float ExoSpotter::Exoplanet::findRadiusRatio()
{
	if (m_averageFlux > 0) {
		return std::sqrt(1 - m_averageFlux);
	}
	
	return -1;
}


std::ostream& ExoSpotter::operator<<(std::ostream& strm, const ExoSpotter::Exoplanet& exoplanet) {
	Exoplanet planet = exoplanet;
	
	return strm << "First observed transit date: " << planet.datapoints().date()[0] <<
				   "\nFirst observed flux: " << planet.datapoints().flux()[0] <<
				   "\nAverage flux: " << planet.m_averageFlux <<
				   "\nPeriod: " << planet.m_averagePeriod << " days\n";
}


/*
Filters through the data to find datapoints under
the value potentialPlanetThreshold
*/
ExoSpotter::Lightcurve ExoSpotter::FindPlanet::findPlanetCandidates()
{
	std::vector<float> flux;
	std::vector<float> date;

	for (int i = 0; i < rawData.size(); i++) {
		if (rawData.flux()[i] <= potentialPlanetThreshold) {
			flux.push_back(rawData.flux()[i]);
			date.push_back(rawData.date()[i]);
		}
	}

	return {flux, date};
}


/*
Groups datapoints from the same transit into one datapoint with the peak
datapoint with the corresponding Julian date.
*/
ExoSpotter::Lightcurve ExoSpotter::FindPlanet::groupDatapoints(Lightcurve data)
{
	Lightcurve groupedData;

	float peakFlux = 2;
	float peakFluxDate = 0;
	float startDate = 0;

	for (int i = 0; i < data.size(); i++) {
		// if a new transit has occured
		if (data.date()[i] - startDate > maxTransitDurationDays) {
			// If the values are not the default values
			if (!(peakFlux == 2 && peakFluxDate == 0)) {
				groupedData.addPair(peakFlux, peakFluxDate);
			}

			peakFlux = data.flux()[i];
			peakFluxDate = data.date()[i];
			startDate = data.date()[i];
		}

		if (data.flux()[i] < peakFlux) {
			peakFlux = data.flux()[i];
			peakFluxDate = data.date()[i];
		}
	}

	// If the values are not the default values
	if (!(peakFlux == 2 && peakFluxDate == 0)) {
		groupedData.addPair(peakFlux, peakFluxDate);
	}

	return groupedData;
}


int ExoSpotter::FindPlanet::findClosestIndex(Lightcurve data, int startIndex, float targetDate)
{
	int closestIndex = startIndex;
	float lastDiff = abs(data.date()[closestIndex] - targetDate);

	// Find the datapoint closest to the next expected transit
	for (int i = startIndex; i < data.size(); i++) {

		if (abs(data.date()[i] - targetDate) > lastDiff) {
			break;
		}

		lastDiff = abs(data.date()[i] - targetDate);
		closestIndex = i;
	}

	return closestIndex;
}


/*
Splits the grouped datapoints into different unordered maps by the
size of the planet. Used for checking if the period between the data
is the same, meaning that it is a planet.
*/
std::vector<ExoSpotter::Lightcurve> ExoSpotter::FindPlanet::splitDatapoints(Lightcurve data)
{
	std::vector<Lightcurve> splitData;

	for (int i = 0; i < data.size(); i++) {
		bool broken = false;

		for (int j = 0; j < splitData.size(); j++) {
			if (abs(splitData[j].flux()[0] - data.flux()[i]) <= samePlanetSizeThreshold) {
				splitData[j].addPair(data.flux()[i], data.date()[i]);

				broken = true;
				break;
			}
		}

		if (broken) {
			continue;
		}

		splitData.push_back(Lightcurve{ {data.flux()[i]}, {data.date()[i]} });
	}

	return splitData;
}


/*
Returns a vector of all detected planets in a dataset splitted, grouped.
*/
std::vector<ExoSpotter::Exoplanet> ExoSpotter::FindPlanet::planetsInData(Lightcurve data)
{
	if (data.size() < 3) {
		return {};
	}

	std::vector<Exoplanet> detectedExoplanets;

	for (int i = 0; i < data.size(); i++) {
		for (int j = i + 1; j < data.size(); j++) {
			float period = data.date()[j] - data.date()[i];
			float nextExpectedTransit = data.date()[j] + period;

			int closestIndex = findClosestIndex(data, j, nextExpectedTransit);
			float closest = data.date()[closestIndex];

			// if the dist between the next expected transit and the closest acutal transit is
			// less than the TTVRange, then it is a possible planet.
			if (fabs(closest - nextExpectedTransit) > TTVRange) {
				continue;
			}

			int expectedTransits = (data.date()[data.size() - 1] - data.date()[0]) / period;
			int missedTransits = (data.date()[i] - rawData.date()[0]) / period;

			std::vector<float> foundTransitFluxes = { data.flux()[i], data.flux()[j], data.flux()[closestIndex] };
			std::vector<float> foundTransitDates = { data.date()[i], data.date()[j], data.date()[closestIndex] };

			for (int iter = 1; nextExpectedTransit + period < data.date()[data.size() - 1]; iter++) {
				nextExpectedTransit += period;

				int closestIndex = findClosestIndex(data, j, nextExpectedTransit);
				float closest = data.date()[closestIndex];

				if (fabs(closest - nextExpectedTransit) > TTVRange * iter * 0.75) {
					missedTransits++;
				}

				else {
					foundTransitFluxes.push_back(data.flux()[closestIndex]);
					foundTransitDates.push_back(closest);
				}
			}

			Exoplanet candidate = Exoplanet{
				Lightcurve{ foundTransitFluxes, foundTransitDates },

				1 - (float)missedTransits / (float)expectedTransits
			};

			bool broken = false;

			// If more than allowedMissedTransits were missed, exclude this planet. This is to
			// prevent noise data from being considered a real planet.
			if (candidate.confidence() >= minimumConfidence) {
				bool broken = false;

				for (Exoplanet exoplanet : detectedExoplanets) {
					Exoplanet& greatest = exoplanet;
					Exoplanet& smallest = candidate;

					if (candidate.averagePeriod() > exoplanet.averagePeriod()) {
						greatest = candidate;
						smallest = exoplanet;
					}

					if (fabs(greatest.averagePeriod() - smallest.averagePeriod() * round(greatest.averagePeriod() / smallest.averagePeriod())) < TTVRange) {
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

	return detectedExoplanets;
}

void ExoSpotter::FindPlanet::printVerbose(
	Lightcurve candidates, Lightcurve grouped, std::vector<Lightcurve> splitted, std::vector<Exoplanet> planets)
{
	std::cout << "*** Candidates ***\n\n";

	for (int i = 0; i < candidates.size(); i++) {
		std::cout << candidates.flux()[i] << "," << candidates.date()[i] << "\n";
	}

	std::cout << "\n\n*** Grouped ***\n\n";

	for (int i = 0; i < grouped.size(); i++) {
		std::cout << grouped.flux()[i] << "," << grouped.date()[i] << "\n";
	}

	for (int i = 0; i < splitted.size(); i++) {
		std::cout << "\n\n*** SPLIT GROUP " << i << " ***\n\n";

		for (int j = 0; j < splitted[i].size(); j++) {
			std::cout << splitted[i].flux()[j] << "," << splitted[i].date()[j] << "\n";
		}
	}

	std::cout << "\n\n*** Planet Fluxes *** \n\n";

	for (auto planet : planets) {
		std::cout << planet << "\n";
	}

	std::cout << "\n";
}


/*
Uses a different algorithm to find more exoplanets at the expense of time
*/
std::vector<ExoSpotter::Exoplanet> ExoSpotter::FindPlanet::findPlanets(bool verbose)
{
	auto candidates = findPlanetCandidates();
	auto grouped = groupDatapoints(candidates);
	auto splitted = splitDatapoints(grouped);
	
	std::vector<Exoplanet> planets;

	for (auto splitData : splitted) {
		std::vector<Exoplanet> exoplanets = planetsInData(splitData);

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
ExoSpotter::FindPlanet::FindPlanet(Lightcurve data, float planetThreshold, 
	float sizeThreshold, float maxTransitDurationDays, float TTVRange, float minimumConfidence)
{
	this->rawData = data;

	potentialPlanetThreshold = planetThreshold;
	samePlanetSizeThreshold = sizeThreshold;
	this->maxTransitDurationDays = maxTransitDurationDays;
	this->TTVRange = TTVRange;
	this->minimumConfidence = minimumConfidence;
}