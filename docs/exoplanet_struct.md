# What is the Exoplanet struct?

The `Exoplanet` struct is used as the return value of the `FindPlanet` class. It contains important information about the exoplanet detected such as the orbital period of the planet.

# Data contained in the Exoplanet struct

The exoplanet struct contains three main values:
- `float averageFlux`: The average normalized flux of the planet. Normalized flux means that 1 = average brightness of the star and 0 = no light is reaching the sensor.
- `float averagePeriod`: The average orbital period of the planet.
- `Lightcurve datapoints`: Specific datapoints the program found from the planet.
- `float confidence`: Percent confidence of an exoplanet between 0 and 1. **Note that "1" does not mean that it is certain to be an exoplanet, but the program thinks it is probable to be an exoplanet.**

These values can be accessed by doing:
```cpp
#include "include/exoSpotter/exoSpotter.h"

int main()
{
    ExoSpotter::Exoplanet myExoplanet = ...;

    float flux = myExoplanet.averageFlux;
    float period = myExoplanet.averagePeriod;
    ExoSpotter::Lightcurve datapoints = myExoplanet.datapoints;
}

```

# Find the Semi-Major Axis

To find the [semi-major axis](https://en.wikipedia.org/wiki/Semi-major_and_semi-minor_axes) of the orbit in [astronomical units (AU)](https://en.wikipedia.org/wiki/Astronomical_unit), run the method `findSemiMajAxis(float solarMasses)`, which returns the semi-major axis of the exoplanet as a float.

```cpp
ExoSpotter::Exoplanet myExoplanet = ...;

// the star's mass in solar masses
float starMass = 2.3;

// the semi major axis of the planet in AU
float semiMajAxis = myExoplanet.findSemiMajAxis(starMass); 
```

# Find the radius ratio

To find the ratio between the planet's radius and the host star's radius, use the `findRadiusRatio()` method. If the ratio cannot be found (usually due to a division by zero error), the method will return `-1`.

Usage looks like the following:
```cpp
#include "include/exoSpotter/exoSpotter.h"

int main()
{
    ExoSpotter::Exoplanet myExoplanet = ...;

    const float starRadius = ...  // optional: the host star's radius
    float radiusRatio = myExoplanet.findRadiusRatio();

    if (radiusRatio != -1) {
        float planetRadius = radiusRatio * starRadius;  // find the radius of the planet in the units given for starRadius
    }
}
```

# Constructor

When working on developing the library, it is essential to fully understand the values of the Exoplanet constructor. However, if you are only using this library and not developing it, this is not required to know.

The Exoplanet struct only requires a `Lightcurve` struct.

Here is an example of initializing the Exoplanet struct:
```cpp
#include "include/exoSpotter/exoSpotter.h"

int main()
{
    // Create a lightcurve struct to input into the Exoplanet constructor.
    // If you haven't already, see lightcurve_struct.md to understand how to
    // initalize that struct.
    ExoSpotter::Lightcurve planetData{ ... };

    ExoSpotter::Exoplanet myExoplanet{ planetData };

    return 0;
}
```