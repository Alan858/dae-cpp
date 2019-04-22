/*
 * TODO: Description of the class
 */

#pragma once

#include "typedefs.h"

namespace daecpp_namespace_name
{

class SolverOptions
{
public:
    /*
     * List of public solver options
     */
    double atol = 1.0e-6;  // Absolute tolerance
    double dt_init = 0.1;  // Initial time stem

    SolverOptions() = default;
};

}  // namespace daecpp_namespace_name