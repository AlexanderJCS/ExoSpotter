# What is the Lightcurve struct?

The lightcurve struct stores [light curve](https://en.wikipedia.org/wiki/Light_curve) data.

**These vectors always have equal length.**

# Lightcurve constructor

Understanding how the lightcurve constructor is not essential for using this library, but it is important when modifying source code.

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

# Getting Data

## Getting Flux

To get a vector of all the flux data, use the `.flux()` method. Usage looks like so:
```cpp
ExoSpotter::Lightcurve my_lc = ...;

std::vector<float> fluxes = my_lc.flux();
```

## Getting Date

Using the date getter method works effectively the same as getting the fluxes. This method returns a vector of all the dates. Usage looks like so:
```cpp
ExoSpotter::Lightcurve my_lc = ...;

std::vector<float> dates = my_lc.date();
```

# Size Method

To get the length of the data in the Lightcurve object, use the `.size()` method. The method returns a `size_t` type.

# Slice method

The slide method takes in two arguments:
- `beginIndex`
- `endIndex`

This method is useful for getting a portion of the full Lightcurve data.

# Add Pair

The `.addPair()` method is overloaded. Both methods take in two arguments, `flux` and `date` respectively. These datapoint(s) are added to the end of the struct's flux and date vectors, appending to them.

## vector<float> Overload

The `vector<float>` overload of this method has the following parameters:
```cpp
void ExoSpotter::Lightcurve::addPair(std::vector<float> flux, std::vector<float> date);
```

A `std::invalid_arguments` exeption will be thrown if the flux and date sizes are unequal.

## float Overload

The float overload of this method has the following parameters:
```cpp
void ExoSpotter::Lightcurve::addPair(float flux, float date);
```

This method is essentially the same thing as having each of the arguments in a one-length vector and passing the variable through the `vector<float>` overload, however this provides a more concise way of doing so.
