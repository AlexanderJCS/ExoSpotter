# exoplanet-finder
ExoSpotter is a C++ library to find planets outside our solar system given the time-series data of the percieved brightness (flux) of a star. This library uses the [transit method](http://www.exoplanetes.umontreal.ca/transit-method/?lang=en) to detect exoplanets.

**This library is made for C++17 and above.**

To start out, it is recommended you review:
- [how to parse data](https://github.com/AlexanderJCS/ExoSpotter/blob/main/docs/parse_data.md)
- [the FindPlanet class](https://github.com/AlexanderJCS/ExoSpotter/blob/main/docs/find_planet_class.md)
- [and the Exoplanet struct](https://github.com/AlexanderJCS/ExoSpotter/blob/main/docs/exoplanet_struct.md)


# Table of Contents

```
1. exoplanet-finder (info about this project)
2. Contributing
    a. Test tools
    b. Issues
    c. Pull requests
````

# Contributing

## Test tools

Under the `test` directory, there will be a file named `test.cpp`. This file is to make sure that the code compiles and the library runs correctly. It is also recommended to replace `data.csv` with real data. In its current state, `data.csv` is a placeholder for real data.

## Issues

Suggestions are welcome in the issues section.

For bugs, please state what the issue is, the data you used (if you can share it that would be great), and the code you created that has the issue.

## Pull requests

For minor changes, please submit a pull request and state what you changed. For major changes, please create an issue to discuss the change before creating a pull request.
