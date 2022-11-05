# Converting .fits data to .csv

First, make sure that the latest version of Python 3 is installed as well as astropy version 3.0.4 or greater. You can install astropy by performing the command:
```
$ python -m pip install astropy
```

To convert lightcurve data in the style of Flexible Image Transport System (FITS) data to .csv data to be read by the program, navigate to tools/fits_to_csv. Then, move your fits data to the same directory as the Python file. Finally, run the file and provide it all the information that is needed. The output of this program will be a file named `data.csv`, which can be fed into `parseData.h`.

If you want to download a direct link to a MAST URL to download TESS satellite data to test this program, do so [here](https://archive.stsci.edu/missions/tess/tid/s0001/0000/0000/2515/5310/tess2018206045859-s0001-0000000025155310-0120-s_lc.fits). This link is the MAST URL for the star WASP-126.

If you want to download data for another star, follow this guide under the instructions "post-search filter" [here](https://outerspace.stsci.edu/display/TESS/6.1.6+-+Searching+For+And+Retrieving+TESS+Mission+Products).


# Converting .csv data to a Lightcurve object

Using the parseData header makes it simple to get the data from a csv into the correct format to be read by the library. You can get the data into this format by reviewing the `Converting .fits data to .csv data` section.

By using `parseData.h`, we can write the following code:
```cpp
#include "include/exoSpotter/parseData.h"

int main()
{
    ExoSpotter::Lightcurve data = readData("filename.csv");  // replace "filename" with your file's name
}
```
