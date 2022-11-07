# ExoSpotter
ExoSpotter is a C++ library to find planets outside our solar system given the time-series data of the percieved brightness (flux) of a star. This library uses the [transit method](http://www.exoplanetes.umontreal.ca/transit-method/?lang=en) to detect exoplanets.

**This library is made for C++17 and above.**

To start out, it is recommended you review:
- [how to parse data](docs/parse_data.md)
- [the FindPlanet class](docs/find_planet_class.md)
- [and the Exoplanet struct](docs/exoplanet_struct.md)

Additionally, it is also recommended to view [the Lightcurve struct](docs/lightcurve_struct.md)

# Screenshots

The program identifying WASP 126-b and WASP 126-c using TESS lightcurve data:
![image](https://user-images.githubusercontent.com/98898166/200208273-1ac28f31-9c93-4077-860e-0b6d323113a9.png)


# Contributing

## Test tools

Under the `test` directory, there will be a file named `test.cpp`. This file is to make sure that the code compiles and the library runs correctly. To run the test, you need to [parse .fits data to csv to be read by the program](docs/parse_data.md).

## Issues

Suggestions are welcome in the issues section.

For bugs, please state what the issue is, the data you used (if you can share it that would be great), and the code you created that has the issue.

## Pull requests

For minor changes, please submit a pull request and state what you changed. For major changes, please create an issue to discuss the change before creating a pull request.
