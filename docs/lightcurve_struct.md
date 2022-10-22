# What is the Lightcurve struct?

The lightcurve struct stores [light curve](https://en.wikipedia.org/wiki/Light_curve) data. More specifically, it contains two vectors which can be used to represent light curve data:

- `std::vector<float> flux` The flux for the corresponding `date` float, which has the same index
- `std::vector<float> date`: The Julian date of the datapoint in `flux` at the same index

**These vectors should have equal length.**

# Lightcurve constructor

Understanding how the lightcurve constructor is not essential for using this library, but it is essentiatl for modifying source code.

## Non-empty constructor

The Lightcurve constructor has two arguments, `std::vector<float> flux` and `std::vector<float> date`. An example of initalizing the Lightcurve object looks like this:

```cpp
std::vector<float> fluxes = { 1.0,  0.99, 0.99,  1.0, 1.05 };
std::vector<float> dates = { 142.5, 143,  143.5, 144, 144.5 };

ExoSpotter::Lightcurve myLightcurve{ fluxes, dates };
```

**A `std::invalid_argument` error will be thrown if the flux and date vectors are not the same size.**

## Empty constructor

The purpose of the empty Lightcurve constructor is to allow initialization of the struct without adding any values into the `flux` and `date` variables. Calling the empty constructor is effectively the same thing as running `ExoSpotter::Lightcurve myLightcurve{{}, {}}`, but it provides a more conscise way of doing so.