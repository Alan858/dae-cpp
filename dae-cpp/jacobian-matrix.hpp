/*
 * Jacobian matrix class.
 * Defines the Jacobian matrix (matrix of the RHS derivatives) for the DAE system `M dx/dt = f`.
 * This class is abstract and must be inherited.
 *
 * This file is part of dae-cpp.
 *
 * dae-cpp is licensed under the MIT license.
 * A copy of the license can be found in the LICENSE file.
 *
 * Copyright (c) 2024 Ivan Korotkin
 */

#ifndef DAECPP_JACOBIAN_MATRIX_H
#define DAECPP_JACOBIAN_MATRIX_H

#include <utility> // std::pair

#include "sparse-matrix.hpp"
#include "vector-function.hpp"

namespace daecpp_namespace_name
{

/*
 * Jacobian matrix class.
 * This class is abstract and must be inherited.
 */
class JacobianMatrix
{
public:
    /*
     * Defines the Jacobian matrix (matrix of the RHS derivatives) for the DAE system `M dx/dt = f`.
     * Takes vector x and time t and returns the Jacobian matrix J.
     * Matrix J is empty and should be filled with non-zero elements.
     * This function is pure virtual and must be overriden to provide analytical Jacobian.
     */
    virtual void operator()(sparse_matrix &J, const state_vector &x, const double t) const = 0;

    virtual ~JacobianMatrix() {}
};

/*
 * Helper Jacobian shape class.
 * Computes Jacobian matrix from the given shape (a list of non-zero elements) provided by the user.
 * This class is abstract and must be inherited.
 */
template <class RHS>
class JacobianMatrixShape
{
    // Array of non-zero elements
    std::vector<std::pair<int_type, int_type>> m_Jn;

    // An estimation of the array (sparse Jacobian matrix) size
    int_type m_N_elements{0};

    // The RHS for differentiation (a copy)
    RHS m_rhs;

public:
    explicit JacobianMatrixShape(RHS rhs) : m_rhs(rhs) {}

    virtual ~JacobianMatrixShape() {}

    /*
     * Defines the Jacobian matrix (matrix of the RHS derivatives) for the DAE system `M dx/dt = f`.
     * Loops through all non-zero elements and performs automatic differentiation for each element.
     */
    void operator()(sparse_matrix &J, const state_vector &x, const double t)
    {
        const int_type size = static_cast<int_type>(x.size()); // System size

        state_type x_(size); // Vectors of `dual` numbers are defined with `_` suffix

        // Conversion to dual numbers for automatic differentiation
        for (int_type k = 0; k < size; ++k)
        {
            x_[k] = x[k];
        }

        // Lambda-function with parameters for which the Jacobian is needed
        auto f = [&rhs = m_rhs](const state_type &x_, const double t, const int_type row)
        {
            return rhs.equations(x_, t, row);
        };

        // Reserve memory
        if (m_N_elements)
        {
            J.reserve(m_N_elements);
        }

        // Automatic differentiation of each element marked as non-zero by the user
        for (const std::pair<int_type, int_type> &Jn : m_Jn)
        {
            J(Jn.first, Jn.second, autodiff::derivative(f, wrt(x_[Jn.second]), at(x_, t, Jn.first)));
        }

        // Update sparse Jacobian matrix size estimation
        m_N_elements = static_cast<int_type>(m_Jn.size());
    }

    /*
     * Adds next non-zero element (i, j), where `i` is the row, and `j` is the column in the Jacobian matrix.
     */
    void add_element(const int_type ind_i, const int_type ind_j)
    {
        m_Jn.emplace_back(std::make_pair(ind_i, ind_j));
    }

    /*
     * Adds a row of non-zero elements (i, j_k), where `i` is the row index, and `j_k` is the array of columns.
     */
    void add_element(const int_type ind_i, const std::vector<int_type> ind_j)
    {
        for (const int_type &j : ind_j)
        {
            m_Jn.emplace_back(std::make_pair(ind_i, j));
        }
    }

    /*
     * Clear arrays of non-zero elements
     */
    inline void clear()
    {
        m_Jn.clear();
    }

    /*
     * Reserve memory for the array of non-zero elements and the Jacobian matrix
     */
    inline void reserve(const int_type N_elements)
    {
        m_N_elements = N_elements;
        m_Jn.reserve(N_elements);
    }
};

/*
 * Helper automatic (algorithmic) Jacobian class.
 * Performs algorithmic differentiation of the RHS using `autodiff` package.
 */
template <class RHS>
class JacobianAutomatic
{
    RHS m_rhs; // The RHS for differentiation (a copy)

public:
    explicit JacobianAutomatic(RHS rhs) : m_rhs(rhs) {}

    /*
     * Automatic (algorithmic) Jacobian.
     * Performs algorithmic differentiation of the RHS using `autodiff` package.
     */
    void operator()(sparse_matrix &J, const state_vector &x, const double t)
    {
        const int_type size = static_cast<int_type>(x.size()); // System size

        state_type x_(size); // Vectors of `dual` numbers are defined with `_` suffix

        // Conversion to dual numbers for automatic differentiation
        for (int_type k = 0; k < size; ++k)
        {
            x_[k] = x[k];
        }

        // Vector lambda-function with parameters for which the Jacobian is needed
        auto f = [&rhs = m_rhs, size](const state_type &x_, const double t)
        {
            state_type f_(size);
            rhs(f_, x_, t);
            return f_;
        };

        // Dense Jacobian matrix generated by `autodiff`
        Eigen::MatrixXd jac = autodiff::jacobian(f, wrt(x_), at(x_, t));

        // Convert dense matrix to sparse format
        for (int_type j = 0; j < size; ++j)
        {
            for (int_type i = 0; i < size; ++i)
            {
                const double val = jac(i, j);

                if (std::abs(val) > DAECPP_SPARSE_MATRIX_ELEMENT_TOLERANCE)
                {
                    J(i, j, val); // Jacobian
                }
            }
        }
    }
};

} // namespace daecpp_namespace_name

#endif // DAECPP_JACOBIAN_MATRIX_H
