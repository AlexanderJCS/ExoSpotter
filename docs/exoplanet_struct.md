# What is the Exoplanet struct?

The `Exoplanet` struct is used as the return value of the `FindPlanet` class. It contains important information about the exoplanet detected such as the orbital period of the planet.

# Data contained in the Exoplanet struct

The exoplanet struct contains three main values:
- `bool isPlanet`: Signifies if the data is representing a planet or if it is junk data. This should almost always be false.
- `float averageFlux`: The average normalized flux of the planet. Normalized flux means that 1 = average brightness of the star and 0 = no light.
- `float averagePeriod`: The average orbital period of the planet.
- `Lightcurve planetDatapoints`: Specific datapoints the program found that signifies the exoplanet's existence.

These values can be accessed by doing:
```cpp
Exoplanet myExoplanet = ...;

float flux = myExoplanet.averageFlux;
float period = myExoplanet.averagePeriod;
Lightcurve datapoints = myExoplanet.planetDatapoints;
```

# Constructor

When working on developing the library, it is essential to fully understand the values of the Exoplanet constructor. However, if you are only using this library and not developing it, you can skip this section.

## Non-empty Constructor Arguments

To input data into the constructor, the Exoplanet struct only requires a `Lightcurve` struct.

Here is an example of initializing the Exoplanet struct:
```cpp

// Create a lightcurve struct to input into the Exoplanet constructor.
// If you haven't already, see lightcurve_struct.md to understand how to
// initalize that struct.
ExoplanetFinder::Lightcurve planetData{ ... };

ExoplanetFinder::Exoplanet myExoplanet{ planetData };
```

## Empty Constructor Arguments

The empty constructor of the Exoplanet struct is only used as a return value from a function when no exoplanet was found. An example of this is the private method `ExoplanetFinder::FindPlanet::planetInData` method.

To use this empty constructor, do the following:

```cpp
ExoplanetFinder::Exoplanet myExoplanet{};
```

# Find the Semi-Major Axis

To find the semi-major axis of the orbit in astronomical units (AU), run the method `findSemiMajAxis(float solarMasses)`, which returns a float.

An example of this:
```cpp
ExoplanetFinder::Exoplanet myExoplanet = ...;

// the star's mass in solar masses
float starMass = 2.3;

// the semi major axis of the planet in AU
float semiMajAxis = myExoplanet.findSemiMajAxis(starMass); 
```

# Find the radius ratio

To find the ratio between the planet's radius and the host star's radius, use the `findRadiusRatio()` method. If the ratio cannot be found (usually due to a division by zero error), the method will return `-1`.

Usage looks like the following:
```cpp
ExoplanetFinder::Exoplanet myExoplanet = ...;

const float starRadius = ...  // optional: the host star's radius
float radiusRatio = myExoplanet.findRadiusRatio();

if (radiusRatio != -1) {
    float planetRadius = radiusRatio * starRadius;  // find the radius of the planet in the units given for starRadius
}
```