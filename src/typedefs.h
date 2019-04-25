/*
 * TODO: Description
 */

#pragma once

#include <mkl_types.h>
#include <vector>

#define daecpp_namespace_name daecpp

namespace daecpp_namespace_name
{

typedef double                  float_type;
typedef std::vector<float_type> state_type;
typedef std::vector<MKL_INT>    vector_type_int;

struct sparse_matrix_holder
{
    state_type      A;   // Non-zero elements of the sparse matrix A
    vector_type_int ia;  // Points to the first column index of the given row
                         // in the array ja
    vector_type_int ja;  // Contains column indices of the sparse matrix A
};

}  // namespace daecpp_namespace_name
