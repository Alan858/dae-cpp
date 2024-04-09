/*
 * Solution Manager classes.
 * Solution Manager functor will be called every time step providing the time `t` and
 * the corresponding solution `x` for further post-processing.
 * If the functor returns an integer != 0 (`true`), the computation will immediately stop.
 *
 * This file is part of dae-cpp.
 *
 * dae-cpp is licensed under the MIT license.
 * A copy of the license can be found in the LICENSE file.
 *
 * Copyright (c) 2024 Ivan Korotkin
 */

#ifndef DAECPP_SOLUTION_MANAGER_H
#define DAECPP_SOLUTION_MANAGER_H

#include <algorithm> // for `binary_search` and `sort`

#include "typedefs.hpp"

namespace daecpp_namespace_name
{

/*
 * Default Solution Manager class
 */
struct SolutionManager
{
    /*
     * Solution Manager functor will be called every time step providing the time `t` and
     * the corresponding solution `x` for further post-processing.
     * If the functor returns an integer != 0 (`true`), the computation will immediately stop.
     * It does nothing by default and returns 0.
     */
    virtual int operator()(const state_vector &x, const double t)
    {
        return 0;
    }

    virtual ~SolutionManager() {}
};

/*
 * Solution holder class.
 * Holds solution vectors `x` and the corresponding times `t`.
 */
struct SolutionHolder
{
    // Vector of solution vectors `x`
    std::vector<state_vector> x;

    // Vector of the corresponding solution times `t`
    std::vector<double> t;

    /*
     * A helper function to print `x` and `t` on screen.
     * By default, prints the entire vector `x` and the corresponding time `t`.
     * Optional parameter: vector of solution indices for printing (`std::vector<std::size_t>`).
     *
     * Example:
     *     print({0, 1, 4});  // Prints 4 columns: `t`, `x[0]`, `x[1]`, `x[4]` for each time `t`
     */
    void print(const std::vector<std::size_t> &ind = {}) const
    {
        ASSERT(t.size() == x.size(), "Solution vector x size is not equal to the solution time t vector size.");

        // There's nothing to print
        if (t.size() == 0)
        {
            return;
        }

        // Solution vector size
        const std::size_t N = x[0].size();

        // Vector of indices for output
        std::vector<std::size_t> ind_out;

        // Check and fill indices for output
        if (ind.size())
        {
            // The user-defined list may contain duplicates and out-of-range values
            ind_out.reserve(N);
            for (const auto &i : ind)
            {
                if (i < N)
                {
                    ind_out.push_back(i);
                }
            }
        }
        else
        {
            ind_out.resize(N);
            for (std::size_t i = 0; i < N; ++i)
            {
                ind_out[i] = i;
            }
        }

        // Header
        std::cout << "Time";
        for (const auto &i : ind_out)
        {
            std::cout << "\tx[" << i << "]";
        }
        std::cout << '\n';

        // Loop over all rows
        for (std::size_t i = 0; i < t.size(); ++i)
        {
            // Solution at time `t`
            std::cout << t[i];
            for (const auto &i_out : ind_out)
            {
                std::cout << '\t' << x[i][i_out];
            }
            std::cout << '\n';
        }
    }

    // TODO: Save to a file
    // void save(const char delimiter = ',')
};

/*
 * Solution class.
 * Writes solution vector `x` and time `t` every time step or every specific time
 * from `t_output` vector into vectors `x` and `t`.
 */
class Solution : public SolutionManager
{
    // A reference to the solution holder object
    SolutionHolder &_sol;

    // Output times
    std::vector<double> _t_out; // Need a copy for sorting

public:
    /*
     * Writes solution vector `x` and time `t` every time step or every specific time
     * from `t_output` vector into vectors `x` and `t`.
     *
     * Parameters:
     *     `sol` - Solution holder object (SolutionHolder)
     *     `t_output` - (optional) a vector of output times for writing (`std::vector<double>`)
     */
    explicit Solution(SolutionHolder &sol, const std::vector<double> t_output = {}) : _sol(sol), _t_out(t_output)
    {
        if (_t_out.size() > 0)
        {
            std::sort(_t_out.begin(), _t_out.end());
        }
    }

    /*
     * Solution Manager functor will be called every time step providing the time `t` and
     * the corresponding solution `x` for further post-processing.
     */
    int operator()(const state_vector &x_, const double t_)
    {
        if (_t_out.size() > 0)
        {
            if (!std::binary_search(_t_out.begin(), _t_out.end(), t_))
            {
                return 0;
            }
        }

        _sol.x.emplace_back(x_);
        _sol.t.emplace_back(t_);

        return 0;
    }
};

} // namespace daecpp_namespace_name

#endif // DAECPP_SOLUTION_MANAGER_H
