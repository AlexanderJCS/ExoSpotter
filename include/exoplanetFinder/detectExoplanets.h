#pragma once

#include <vector>
#include <string>


namespace ExoplanetFinder
{
	struct Exoplanet
	{
		bool isPlanet = false;
		float firstTransitDate = 0;
		float flux = 0;
		float period = 0;

		Exoplanet();  // used to return nothing if a planet is not identified
		Exoplanet(float firstTransitDate, float flux, float period);

		friend std::ostream& operator<<(std::ostream& strm, const ExoplanetFinder::Exoplanet& exoplanet);
	};

	struct Lightcurve
	{
		std::vector<float> flux;
		std::vector<float> date;

		Lightcurve();
		Lightcurve(std::vector<float> flux, std::vector<float> date);
	};

	class FindPlanet
	{
		Lightcurve rawData{};

		float potentialPlanetThreshold;
		float samePlanetSizeThreshold;
		float maxTransitDurationDays;
		float TTVRange;

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
		Returns a planet struct of the planet identified.

		WARNING: This algorithm may give a false negative when two planets are in the same dataset.
				 This would happen if two planets are similar size, it wouldn't get
				 picked up by the grouping algorithm.
		*/
		Exoplanet planetInData(Lightcurve data);

		/*
		Returns -1 if a planet is not found. Otherwise returns the date
		of the flux of the of the planet found.

		This method is to provide a more sophisticated algorithm which counteracts the
		warning in the planetInData method, at the cost of time.
		*/
		std::vector<Exoplanet> planetInDataPrecise(Lightcurve data);

	public:
		/*
		Calls other methods necessary to find planets
		*/
		std::vector<Exoplanet> findPlanets(bool verbose);

		/*
		Uses a different algorithm to find more exoplanets at the expense of time
		*/
		std::vector<Exoplanet> findPlanetsPrecise(bool verbose = false);

		/*
		data: The data from the observed star's brightness. It will have two key values of "flux" and "date".

		planetThreshold: The minimum amount the star's brightness can dip under for it to be considered a potential
						 datapoint with a planet inside of it.

		sizeThreshold: The range that the star's appeared brightness can vary and be considered the same planet

		maxTransitDurationDays: Used for grouping data. The amount of time, in days, that the transit can take.

		TTVRange: The amount of variation the period of a planet can have, in days, to be considered the same planet.
		*/
		FindPlanet(Lightcurve data, float planetThreshold, float sizeThreshold, 
			float maxTransitDurationDays, float TTVRange);
	};
}
