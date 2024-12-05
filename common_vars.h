#ifndef COMMON_VARS_H
#define COMMON_VARS_H

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>

#define epsilon 1e-160

using std::make_shared;
using std::shared_ptr;

const double pi = 3.1415926535897932385;

inline double degrees_radians(double degrees)
{
    return (degrees * pi) / 180.0;
}

inline double random_double() {
    // Returns a random real in [0,1).
    return std::rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return int(random_double(min, max+1));
}


#endif 
