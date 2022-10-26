#pragma once

#include <optional>
#include <vector>
#include <string>


namespace ExoSpotter
{
	struct Lightcurve
	{
		Lightcurve() = default;
		Lightcurve(std::vector<float> flux, std::vector<float> date);

		std::vector<float> date() { return m_date; }
		std::vector<float> flux() { return m_flux; }

		void addPair(float flux, float date);
		void addPair(std::vector<float> flux, std::vector<float> date);
		Lightcurve slice(int beginIndex, int endIndex);

		size_t size();

	private:
		std::vector<float> m_flux;
		std::vector<float> m_date;
	};

	struct Exoplanet
	{
		float averageFlux = 0;
		float averagePeriod = 0;
		float confidence = 0;  // value between 0 and 1

		Lightcurve datapoints;

		Exoplanet(Lightcurve planetDatapoints, float confidence);

		/*
		Finds the planet's semi-major axis (orbital radius) given the star's solar masses.
		*/
		float findSemiMajAxis(float starSolarMasses);

		/*
		Finds the ratio of the planet radius / host star radius.
		Returns -1 if the ratio cannot be found (usually due to there being no flux).
		*/
		float findRadiusRatio();

		friend std::ostream& operator<<(std::ostream& strm, const ExoSpotter::Exoplanet& exoplanet);
	};

	class FindPlanet
	{
		Lightcurve rawData{};

		float potentialPlanetThreshold;
		float samePlanetSizeThreshold;
		float maxTransitDurationDays;
		float TTVRange;
		int allowedMissedTransits;
		float minimumConfidence;
		
		int findClosestIndex(Lightcurve data, int startIndex, float targetDate);

		/*
		Filters through the data to find datapoints under
		 the value potentialPlanetThreshold
		*/
		Lightcurve findPlanetCandidates();

		/*
		Groups datapoints from the same transit into one datapoint with the peak
		datapoint with the corresponding Julian date.
		*/
		Lightcurve groupDatapoints(Lightcurve data);

		/*
		Splits the grouped datapoints into different unordered maps by the
		size of the planet. Used for checking if the period between the data
		is the same, meaning that it is a planet.
		*/
		std::vector<Lightcurve> splitDatapoints(Lightcurve data);

		/*
		Returns a vector of all the planets found.

		This method is to provide a more sophisticated algorithm which counteracts the
		warning in the planetInData method, at the cost of time.
		*/
		std::vector<Exoplanet> planetsInData(Lightcurve data);

		void printVerbose(
			Lightcurve candidates, Lightcurve grouped, std::vector<Lightcurve> splitted, std::vector<Exoplanet> planets);

	public:

		/*
		Uses a different algorithm to find more exoplanets at the expense of time
		*/
		std::vector<Exoplanet> findPlanets(bool verbose = false);

		/*
		data: The data from the observed star's brightness. It will have two key values of "flux" and "date".

		planetThreshold: The minimum amount the star's brightness can dip under for it to be considered a potential
						 datapoint with a planet inside of it.

		sizeThreshold: The range that the star's appeared brightness can vary and be considered the same planet

		maxTransitDurationDays: Used for grouping data. The amount of time, in days, that the transit can take.

		TTVRange: The amount of variation the period of a planet can have, in days, to be considered the same planet.
		*/
		FindPlanet(Lightcurve data, float planetThreshold, float sizeThreshold, 
			float maxTransitDurationDays, float TTVRange, float minimumConfidence = 0.4, int allowedMissedTransits = 2);
	};
}
