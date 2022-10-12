#pragma once


#include <unordered_map>
#include <vector>
#include <string>


namespace DetectExoplanets
{
	class FindPlanet
	{
		std::vector<float> flux;
		std::vector<float> date;

		float potentialPlanetThreshold;
		float samePlanetSizeThreshold;
		float maxTransitDurationDays;
		float TTVRange;

		/*
		Filters through the data to find datapoints under
		 the value potentialPlanetThreshold
		*/
		std::unordered_map<std::string, std::vector<float>> findPlanetCandidates();

		/*
		Groups datapoints from the same transit into one datapoint with the peak
		datapoint with the corresponding Julian date.
		*/
		std::unordered_map<std::string, std::vector<float>> groupDatapoints(
			std::unordered_map<std::string, std::vector<float>> data);

		/*
		Splits the grouped datapoints into different unordered maps by the
		size of the planet. Used for checking if the period between the data
		is the same, meaning that it is a planet.
		*/
		std::vector<std::unordered_map<std::string, std::vector<float>>> splitDatapoints(
			std::unordered_map<std::string, std::vector<float>> data);

		/*
		Returns if there is at least one planet in the data given.

		WARNING: This algorithm may give a false negative when two planets are in the same dataset.
				 This would happen if two planets are similar size, it wouldn't get
				 picked up by the grouping algorithm.
		*/
		bool planetInData(std::vector<float> data);

	public:
		/*
		Calls other methods necessary to find planets
		*/
		std::vector<float> findPlanets(bool verbose = false);

		/*
		data: The data from the observed star's brightness. It will have two key values of "flux" and "date".

		planetThreshold: The minimum amount the star's brightness can dip under for it to be considered a potential
						 datapoint with a planet inside of it.

		sizeThreshold: The range that the star's appeared brightness can vary and be considered the same planet

		maxTransitDurationDays: Used for grouping data. The amount of time, in days, that the transit can take.

		TTVRange: The amount of variation the period of a planet can have, in days, to be considered the same planet.
		*/
		FindPlanet(std::unordered_map<std::string, std::vector<float>> data,
			float planetThreshold, float sizeThreshold, float maxTransitDurationDays,
			float TTVRange);
	};
}
