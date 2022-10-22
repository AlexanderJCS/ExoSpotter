# exoplanet-finder
C++ library to find exoplanets given the percieved brightness percentage (normalized flux) of a star and the Julian date of that datapoint. This library uses the [transit method](http://www.exoplanetes.umontreal.ca/transit-method/?lang=en) to detect exoplanets.

# Table of Contents

```
0. exoplanet-finder (info about this project)
1. Converting .fits data to .csv data
2. Contributing
    a. Test tools
    b. Issues
    c. Pull requests
````

# Converting .fits data to .csv data

First, make sure that Python 3.10 is installed on your computer as well as astropy version 3.0.4 or greater. You can install astropy by performing the command:
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
    ExoplanetFinder::Lightcurve data = readData("filename.csv");  // replace "filename" with your file's name
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
