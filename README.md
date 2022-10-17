# exoplanet-finder
C++ library to find exoplanets given the percieved brightness percentage (normalized flux) of a star and the Julian date of that datapoint.

An example of data would look like this:

```
flux,julian_date
0.999949689,2457801.085
0.999966108,2457801.126
0.999950989,2457801.168
0.999941499,2457801.21
```

Where the left column is the normalized flux (the precieved brightness percent) and the right column is the Julian date. The date doesn't need to be the Julian date exactly, but it does need to have 1 equal to 1 Earth day.

# Table of Contents

```
0. exoplanet-finder (info about this project)
1. Converting .fits data to .csv data
2. Using this library in your own project
    a. Using parseData.h to parse data
    b. Creating the FindPlanet class
    c. What is the Exoplanet struct?
    d. Finding exoplanets!
    e. Finding exoplanets-- deprecated
3. Contributing
    a. Test tools
    b. Issues
    c. Pull requests
````

# Converting .fits data to .csv data

First, make sure that Python 3.10 is installed on your machine as well as astropy version 3.0.4 or greater. You can install astropy by performing the command:
```
$ pip install astropy
```

To convert lightcurve data in the style of Flexible Image Transport System (fits) data to .csv data, which can be read by the program, navigate to tools/fits_to_csv. Then, move your fits data to the same directory as the Python file. Finally, run the file and provide it all the information that is needed. If you are not sure if your flux data is normalized, assume that it isn't. The output of this program will be a file named `data.csv`, which can be fed into `parseData.h`.

If you want to download a direct link to a MAST URL to download TESS data to test this program, do so here:
https://archive.stsci.edu/missions/tess/tid/s0001/0000/0000/2515/5310/tess2018206045859-s0001-0000000025155310-0120-s_lc.fits

This link is lightcurve data for the star WASP-126.

If you want to download data for another star, follow this guide under the instructions "post-search filter":
https://outerspace.stsci.edu/display/TESS/6.1.6+-+Searching+For+And+Retrieving+TESS+Mission+Products

# Using this library in your own project

To use this library in your own project, you first need to load the data into a `ExoplanetFinder::Data` type. The Data struct will have two values: `std::vector<float> date` and `std::vector<float> flux`. The flux is the percent percieved brightness of the star, and the date is the Julian date (or any other date format where 1 = 1 day) of that datapoint. If your data looks like exactly like the example above, you may use the `parseData.h` file to parse the data in the format for the program. If you have a `.fits` file, review the section before this one to convert the data.

## Using parseData.h to parse data

Using the parseData header makes it simple to get the data from a csv into the correct format to be read by the library, assuming that your data looks like the example above with the flux to the left and the Julian date to the right. You can get the data into this format by reviewing the `Converting .fits data to .csv data` section.

By using `parseData.h`, we can write the following code:
```cpp
#include "include/exoplanetFinder/parseData.h"

int main()
{
    ExoplanetFinder::Data data = readData("filename.csv");  // replace "filename" with your file's name
}
```

We will get a `Data` type in the output.


## Creating the FindPlanet class

To create the FindPlanet class, we need to pass in the five required input variables:
`Data data`: The data aquired using the last step
`float planetThreshold`: The minimum amount the flux can dip under for it to be considered a potential datapoint with a planet inside of it.
`float sizeThreshold`: The range that the flux can vary and be considered the same planet
`float maxTransitDurationDays`: The maximum amount of time, in days, that the transit can take.
`float TTVRange`: The amount of variation the period of a planet can have, in days, to be considered the same planet.

Using this information, we can create the planet class:
```cpp
#include "include/exoplanetFinder/detect_exoplanets.h"

int main()
{
    ExoplanetFinder::Data data = parseData("filename.csv");  // define the data, read the previous section for more info
    DetectExoplanets::FindPlanet planetFinder{ data, 0.9999, 0.002, 1.5, 0.4 };
}
```

## What is the Exoplanet struct?

The `DetectExoplanet::Exoplanet` struct has four variables:
- `bool isPlanet`: If the Exoplanet is detected to be a planet or if it's just junk data. This variable always be true when the struct is sent to the user.
- `float firstTransitDate` is the date first transit the algorithm detected in the data. 
- `float flux` is the lowest value found for the transit at `firstTransitDate`.
- `float period` is the orbital period of the exoplanet around its host star in days.

The struct is primarily used as return type of the `findPlanets()` and `findPlanetsPrecise()` methods in the `FindPlanet` class.
You can also `cout` this class and output its values to the console in a nice, formatted way by doing `std::cout << myExoplanetStruct`.

## Finding exoplanets!

The last step is to use the `FindPlanet.findPlanetPrecise()` method to find the exoplanets in the dataset. This method returns a `std::vector<DetectExoplanets::Exoplanet>` object. 

There is also an optional `bool verbose` argument that can be passed into this method. This is primarily used for debugging and is not recommended for production code.

```cpp
#include <iostream>

#include "include/exoplanetFinder/detect_exoplanets.h"

int main()
{
    // This code was explained in the previous section
    ExoplanetFinder::Data data = readData("filename.csv");  // replace "filename" with your file's name
    DetectExoplanets::FindPlanet planetFinder{ data, 0.9999, 0.002, 1.5, 0.4 };
    
    // Get the planet fluxes
    std::vector<DetectExoplanets::Exoplanet> planetFluxes = planetFinder.findPlanetsPrecise();
    
    // Output the planet fluxes to the console
    for (auto flux : planetFluxes) {
        std::cout << flux << "\n";
    }
}
```

## Find exoplanets-- deprecated

There is a less precise, deprecated way to find exoplanets using the `FindPlanet.findPlanets()` method. This method has an optional boolean parameter `verbose` mainly used for debugging. This is not recommended for production code. 

Here is some example code:
```cpp
#include <iostream>
#include <vector>

#include "include/exoplanetFinder/detect_exoplanets.h"

int main()
{
    // This code was explained in the previous section
    ExoplanetFinder::Data data = parseData("filename.csv");  // define the data, read the previous section for more info
    DetectExoplanets::FindPlanet planetFinder{ data, 0.9999, 0.002, 1.5, 0.4 };
    
    // Get the planet fluxes
    std::vector<DetectExoplanets::Exoplanet> planetFluxes = planetFinder.findPlanets();
    
    // Output the planet fluxes to the console
    for (auto& flux : planetFluxes) {
        std::cout << flux << "\n";
    }
}
```

# Contributing

## Test tools

Under the `test` directory, there will be one file: `test.cpp`. This file is to make sure that the code compiles and the library runs correctly. It is also recommended to replace `data.csv` with real data. In its current state, `data.csv` is a placeholder for real data.

## Issues

Suggestions are welcome in the issues section.

For bugs, please state what the issue is, the data you used (if you can share it that would be great), and the code you created that has the issue.

## Pull requests

For minor changes, please submit a pull request and state what you changed. For major changes, please create an issue to discuss the change before creating a pull request.
