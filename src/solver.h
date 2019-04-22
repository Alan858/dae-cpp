/*
 * TODO: Description of the class
 */

#pragma once

#include "typedefs.h"
#include "RHS.h"
#include "jacobian.h"
#include "mass_matrix.h"
#include "solver_options.h"

namespace daecpp_namespace_name
{

class Solver
{
    RHS &m_rhs;

    Jacobian &m_jac;

    MassMatrix &m_mass;

    SolverOptions &m_opt;

    const double m_t1;

public:
    /*
     * TODO: Description
     */
    Solver(RHS &rhs, Jacobian &jac, MassMatrix &mass, SolverOptions &opt, const double t1)
        : m_rhs(rhs), m_jac(jac), m_mass(mass), m_opt(opt), m_t1(t1)
    {
    }

    /*
     * TODO: Description
     */
    void operator()(state_type &x);
};

}  // namespace daecpp_namespace_name