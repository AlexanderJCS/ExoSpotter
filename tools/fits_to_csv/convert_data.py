import math

try:
    from astropy.io import fits

except ImportError:
    print("The dependency astropy is required to run this program. Please install it by running the "
          "command \"pip install astropy\" in the console.")

    exit(1)

INFO = """
This file's purpose is to convert .fits data into .csv data, which can be read by the
C++ program. This program is meant to work with TESS satellite lightcuve data. Data
from other sources are not garunteed to work. If you want to download a direct link to
a MAST URL to download TESS data to test this program, do so here:

https://archive.stsci.edu/missions/tess/tid/s0001/0000/0000/2515/5310/tess2018206045859-s0001-0000000025155310-0120-s_lc.fits

This link is lightcurve data for the star WASP-126.

If you want to download data for another star, follow this guide under the instructions "post-search filter":
https://outerspace.stsci.edu/display/TESS/6.1.6+-+Searching+For+And+Retrieving+TESS+Mission+Products

Dependencies: astropy >= 3.0.4
"""


def normalize_data(fluxes: list) -> list:
    average = 0.0

    for flux in fluxes:
        average += flux

    average /= len(fluxes)

    if average == 0:
        raise ZeroDivisionError("The average of the flux data is 0. Is the length of the data greater than 0?")

    return [flux / average for flux in fluxes]


def remove_nan(dates: list, fluxes: list) -> tuple:
    new_dates = []
    new_fluxes = []

    for i in range(len(fluxes)):
        if not math.isnan(fluxes[i]):
            new_dates.append(dates[i])
            new_fluxes.append(fluxes[i])

    return new_dates, new_fluxes


def main():
    print(f"{INFO}\n")
    filename = input("Please input the .fits data's filename or path (file extension needs to be included): ")
    normalized = input("Is the data normalized (where 1 = average flux, 0 = no light)? (y/n): ")

    normalized = normalized.lower() in ("y", "yes")

    with fits.open(filename, mode="readonly") as hdulist:
        tess_bjds = hdulist[1].data['TIME']
        pdcsap_fluxes = hdulist[1].data['PDCSAP_FLUX']

    if len(tess_bjds) != len(pdcsap_fluxes):
        raise IndexError(
            f"Length of date data is {tess_bjds}, which is not equal to length of fluxes ({pdcsap_fluxes})"
        )

    tess_bjds, pdcsap_fluxes = remove_nan(tess_bjds, pdcsap_fluxes)

    # Normalize the data if it is not already normalized
    if not normalized:
        pdcsap_fluxes = normalize_data(pdcsap_fluxes)

    # Write the data to filename data.csv
    with open("data.csv", "w") as f:
        f.write("flux,julian_date\n")

        for i in range(len(tess_bjds)):
            f.write(f"{pdcsap_fluxes[i]},{tess_bjds[i]}\n")

    print("Task successfully completed. Data output is in data.csv")


if __name__ == "__main__":
    main()
