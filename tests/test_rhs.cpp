#include <dae-cpp/rhs.hpp>

#include "gtest/gtest.h"

// Testing:
// class RHS

namespace
{

using namespace daecpp;

TEST(RHS, Definition)
{
    struct TestRHS : public RHS
    {
        void operator()(state_type &f, const state_type &x, const double t) const
        {
            EXPECT_EQ(x.size(), 2);

            f[0] = x[0];
            f[1] = x[1] * t;
        }
    };

    TestRHS rhs;

    state_type x{4.0, 6.0};
    state_type f(2);

    constexpr double t{10.0};

    rhs(f, x, t);

    EXPECT_DOUBLE_EQ(f[0], 4.0);
    EXPECT_DOUBLE_EQ(f[1], 6.0 * t);

    EXPECT_EQ(f.size(), 2);
}

} // namespace
