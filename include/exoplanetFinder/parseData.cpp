#include <unordered_map>
#include <iostream>
#include <fstream>
#include <ctype.h>
#include <vector>
#include <string>



/*
Parses the data to be used in the FindPlanet class.
*/
std::unordered_map<std::string, std::vector<float>> readData(std::string filename)
{
	std::ifstream file{filename};
	
	if (!file.is_open()) {
		std::cout << "could not open file\n";
		exit(1);
	}
	
	std::unordered_map<std::string, std::vector<float>> returnMap;

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
			std::cout << "Error with data with the line " << line << ". No seperator (,) found.";
			exit(1);
		}

		// Push back "flux" by the first value in the csv converted to a float
		returnMap["flux"].push_back(std::stof(line.substr(0, commaIndex)));

		// Same thing here but with "date"
		returnMap["date"].push_back(std::stof(line.substr(commaIndex + 1)));
	}
	

	return returnMap;
}