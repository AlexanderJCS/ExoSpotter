# What is the FindPlanet class?

The FindPlanet class the main way you, the developer, interact with this library. The class takes six input variables that determine *how* it finds planets in the data.

# Initializing the FindPlanet class

To create the FindPlanet class, we need to pass in the six required input variables:

- `Lightcurve data`: The data aquired using the `parseData` header (look at [parse_data.md](parse_data.md) for more info).
- `float planetThreshold`: The minimum percentage the flux can go under for it to be considered a potential exoplanet transit.
- `float sizeThreshold`: The range that the flux can vary and be considered the same planet.
- `float maxTransitDurationDays`: 2 times the maximum time the transit can take, in days. E.g. if the transit takes 0.5 days max, this value should be 0.5 * 2 = 1.
- `float TTVRange`: The amount of variation the period of a planet can have, in days, to be considered the same planet. The recommended value for this is around 0.5.
- `int allowedMissedTransits = 2`: This defaults to 2 unless otherwise specified. This is the number of transits that can be missed before the detected transit still be counted as an exoplanet. This is made to reduce false positives in noisy data. Unless you have a specific reasion, it is recommended to keep this at 2.
- `float minimumConfidence = 0.4`: Defaults ot 0.4 unless otherwise specified. This is the minimum confidence the program can have in an exoplanet and still include it in the final list of planets.

Using this information, we can create the planet class:

```cpp
#include "include/exoSpotter/exoSpotter.h"
#include "include/exoSpotter/parseData.h"

int main()
{
    ExoSpotter::Lightcurve data = parseData("filename.csv");  // define the data, read parse_data.md for more info
    DetectExoplanets::FindPlanet planetFinder{ data, 0.9999, 0.002, 1.5, 0.4, 2 };
}
```


# Finding Exoplanets

The last step is to use the `FindPlanet.findPlanetPrecise()` method to find the exoplanets in the dataset. This method returns a `std::vector<ExoSpotter::Exoplanet>` object.

There is also an optional `bool verbose` argument that can be passed into this method. This is primarily used for debugging and is not recommended for production code.

```cpp
#include <iostream>

#include "include/exoSpotter/exoSpotter.h"
#include "include/exoSpotter/parseData.h"

int main()
{
    // This code is explained in parse_data.md section
    ExoSpotter::Lightcurve data = readData("filename.csv");  // replace "filename" with your file's name
    
    // Initalize the FindPlanet class
    ExoSpotter::FindPlanet planetFinder{ data, 0.9999, 0.003, 1.0, 0.1, 2 };
    
    // Get a vector of all exoplanets identified
    std::vector<ExoSpotter::Exoplanet> planets = planetFinder.findPlanetsPrecise();
    
    // Output the planet to the console
    for (const auto planet : planets) {
        std::cout << planet << "\n";
    }
}
```