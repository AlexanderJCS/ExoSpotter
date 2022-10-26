# What is the FindPlanet class?

The FindPlanet class the main way you, the developer, interact with this library. The class takes six input variables that determine *how* it finds planets in the data. **If the input variables are not exactly correct, it may not provide the proper output. It is highly recommended to read the input variables closely.**

# Initializing the FindPlanet class

To create the FindPlanet class, we need to pass in the six required input variables:

- `Data data`: The data aquired using the `parseData` header (look at parse_data.md for more info).
- `float planetThreshold`: The minimum amount the flux can dip under for it to be considered a potential datapoint with a planet inside of it.
- `float sizeThreshold`: The range that the flux can vary and be considered the same planet
- `float maxTransitDurationDays`: The maximum amount of time, in days, that the transit can take. **It is recommended this value is under the minimum orbital period expected, but as close to 1.0 as possible.**
- `float TTVRange`: The amount of variation the period of a planet can have, in days, to be considered the same planet. The recommended value for this is around 0.5.
- `int allowedMissedTransits = 2`: parameter which defaults to 2 unless otherwise specified. This is the number of transits that cna be missed and still be counted as an exoplanet. This is made to reduce false positives in noisy data. Unless you have a specific reasion, it is recommended to keep this at 2.
- `float minimumConfidence = 0.4`: parameter which defaults ot 0.4 unless otherwise specified. This is the minimum confidence the program can have in an exoplanet and still include it in the final list of planets.

Using this information, we can create the planet class:

```cpp
#include "include/exoSpotter/exoSpotter.h"

int main()
{
    ExoSpotter::Data data = parseData("filename.csv");  // define the data, read the previous section for more info
    DetectExoplanets::FindPlanet planetFinder{ data, 0.9999, 0.002, 1.5, 0.4, 2 };
}
```


# Finding Exoplanets!

The last step is to use the `FindPlanet.findPlanetPrecise()` method to find the exoplanets in the dataset. This method returns a `std::vector<DetectExoplanets::Exoplanet>` object.

There is also an optional `bool verbose` argument that can be passed into this method. This is primarily used for debugging and is not recommended for production code.

```cpp
#include <iostream>

#include "include/exoSpotter/exoSpotter.h"


int main()
{
    // This code is explained in parse_data.md section
    ExoSpotter::Data data = readData("filename.csv");  // replace "filename" with your file's name
    
    // Initalize the FindPlanet class
    DetectExoplanets::FindPlanet planetFinder{ data, 0.9999, 0.003, 1.0, 0.1, 2 };
    
    // Get a vector of all exoplanets identified
    std::vector<DetectExoplanets::Exoplanet> planets = planetFinder.findPlanetsPrecise();
    
    // Output the planet to the console
    for (const auto& planet : planets) {
        std::cout << planet << "\n";
    }
}
```

## Find exoplanets: deprecated

There is a less precise, deprecated way to find exoplanets using the `FindPlanet.findPlanets()` method. This method has an optional boolean parameter `verbose` mainly used for debugging. This is not recommended for production code. 

Here is some example code:
```cpp
#include <iostream>
#include <vector>

#include "include/exoSpotter/exoSpotter.h"

int main()
{
    // This code is explained in parse_data.md section
    ExoSpotter::Data data = parseData("filename.csv");  // replace "filename" with your file's name
    
    // Initalize the FindPlanet class
    DetectExoplanets::FindPlanet planetFinder{ data, 0.9999, 0.003, 1.0, 0.1, 2  };
    
    // Get the Exoplanet objects
    std::vector<Exoplanet> planetFluxes = planetFinder.findPlanets();
    
    // Output the planet fluxes to the console
    for (const auto& planet : planets) {
        std::cout << planet << "\n";
    }
}
```