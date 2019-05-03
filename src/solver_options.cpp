/*
 * TODO: Description
 */

#include "solver_options.h"

namespace daecpp_namespace_name
{

/*
 * Intel MKL PARDISO iparm parameters:
 * https://software.intel.com/en-us/mkl-developer-reference-c-pardiso-iparm-parameter
 */
void SolverOptions::set_iparm_for_pardiso(MKL_INT *iparm)
{
    for(MKL_INT i = 0; i < 64; i++)
    {
        iparm[i] = 0;
    }

    iparm[0] = 1;  // No solver default, must supply all values in
                   // components iparm[1] - iparm[63].
    iparm[1] = 3;  // The parallel (OpenMP) version of the nested dissection
                   // algorithm. It can decrease the time of computations on
                   // multi-core computers, especially when Intel MKL
                   // PARDISO Phase 1 takes significant time.

    iparm[3] = preconditioned_CGS;  // Controls preconditioned CGS/CG

    iparm[4] = 0;  // No user fill-in reducing permutation
    iparm[5] = 0;  // Write solution into x
    iparm[6] = 0;  // Reports the number of iterative refinement steps performed

    iparm[7] = refinement_steps;  // Number of iterative refinement steps

    iparm[9]  = 13;  // Perturb the pivot elements with 1.0E-13 (default)
    iparm[10] = 1;   // Enable scaling. Default for nonsymmetric matrices.
    iparm[11] = 0;   // Conjugate transposed/transpose solve
    iparm[12] = 1;   // Maximum weighted matching algorithm is switched-on
    iparm[13] = 0;   // Output: Number of perturbed pivots
    iparm[14] = 0;   // Output: Peak memory on symbolic factorization
    iparm[15] = 0;   // Output: Permanent memory on symbolic factorization
    iparm[16] = 0;   // Output: Size of factors/Peak memory on numerical
                     // factorization and solution
    iparm[17] = 0;   // Report the number of nonzeros in the factor LU
    iparm[18] = 0;   // Report Mflops for LU factorization
    iparm[19] = 0;   // Output: Numbers of CG Iterations

    iparm[23] = parallel_fact_control;  // Parallel factorization control

    if(iparm[23] == 1)
    {
        iparm[10] = 0;  // Disable scaling
        iparm[12] = 0;  // Disable matching
    }

    iparm[24] = parallel_solve_control;  // Parallel forward/backward solve
                                         // control

    iparm[26] = 0;  // Matrix checker
    iparm[27] = 0;  // Single or double precision Intel MKL PARDISO:
                    // 0 - double, 1 - single
    iparm[29] = 0;  // Number of zero or negative pivots
    iparm[30] = 0;  // Partial solve and computing selected components
    iparm[33] = 0;  // Because iparm[1] = 3;
    iparm[34] = 0;  // One- or zero-based indexing of columns and rows:
                    // 0 - one-based, 1 - zero-based
    iparm[59] = 0;  // Intel MKL PARDISO mode - in-core
}

/*
 * Checks correctness of the solver parameters
 */
void SolverOptions::check_options()
{
    if(bdf_order < 1 || bdf_order > BDF_MAX_ORDER)
    {
        // print warning
        // fall back to BDF-1
        bdf_order = 1;
    }
}

}  // namespace daecpp_namespace_name