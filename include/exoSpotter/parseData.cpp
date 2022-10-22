#include <iostream>
#include <fstream>
#include <ctype.h>
#include <string>

#include "exoSpotter.h"

/*
Parses the data to be used in the FindPlanet class.
*/
ExoSpotter::Lightcurve readData(std::string filename)
{
	std::ifstream file{filename};
	
	if (!file.is_open()) {
		std::cout << "\nCould not open file " << filename;
		exit(1);
	}
	
	ExoSpotter::Lightcurve returnData;
	std::string line;
	
	while (file) {
		file >> line;

		// If there is a non-number in the string other than comma, skip the line
		bool skipLine = false;
		
		for (char chr : line) {
			if (chr != ',' && chr != '.' && !isdigit(chr)) {
				skipLine = true;
				break;
			}
		}

		if (skipLine) {
			continue;
		}

		// Separate the line by comma
		auto commaIndex = line.find(',');
		
		if (commaIndex == std::string::npos) {
			std::cout << "\nError with data with the line " << line << ". No seperator (,) found.";
			exit(1);
		}

		// Push back "flux" by the first value in the csv converted to a float
		returnData.flux.push_back(std::stof(line.substr(0, commaIndex)));

		// Same thing here but with "date"
		returnData.date.push_back(std::stof(line.substr(commaIndex + 1)));
	}
	

	return returnData;
}