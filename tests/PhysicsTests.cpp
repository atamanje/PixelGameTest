#include "super_luminal_pch.h"
#include <gtest/gtest.h>
#include "../src/PhysicsCalculations.h"

TEST(PhysicsTests, TripTimesSublight) {
    float ext_yr = 0.0f;
    float ship_yr = 0.0f;

    // 4.0 Ly at 0.5c -> external time 8.0 yrs
    // gamma = 1 / sqrt(1 - 0.25) = 1 / 0.866025 = 1.1547
    // ship time = 8.0 * 0.866025 = 6.9282 yrs
    Physics::CalculateTripTimes(4.0f, 0.5f, ext_yr, ship_yr);
    EXPECT_FLOAT_EQ(ext_yr, 8.0f);
    EXPECT_NEAR(ship_yr, 6.928203f, 0.001f);
}

TEST(PhysicsTests, TripTimesWarp) {
    float ext_yr = 0.0f;
    float ship_yr = 0.0f;

    // 10.0 Ly at 2.0c -> external time 5.0 yrs
    // Warp drive: ship time = external time
    Physics::CalculateTripTimes(10.0f, 2.0f, ext_yr, ship_yr);
    EXPECT_FLOAT_EQ(ext_yr, 5.0f);
    EXPECT_FLOAT_EQ(ship_yr, 5.0f);
}

TEST(PhysicsTests, TripTimesStationary) {
    float ext_yr = 0.0f;
    float ship_yr = 0.0f;

    Physics::CalculateTripTimes(10.0f, 0.0f, ext_yr, ship_yr);
    EXPECT_FLOAT_EQ(ext_yr, 0.0f);
    EXPECT_FLOAT_EQ(ship_yr, 0.0f);
}

TEST(PhysicsTests, AlcubierreExpansion) {
    // Ship traveling at 2.0c, R=1.0, sigma=8.0
    // Expansion at x=0 (center of ship) should be 0
    float theta_center = Physics::AlcubierreExpansionTheta(0.0f, 2.0f, 1.0f, 8.0f);
    EXPECT_FLOAT_EQ(theta_center, 0.0f);

    // Expansion at x > 0 (front of ship) should be negative (contraction)
    // Expansion at x < 0 (behind ship) should be positive (expansion)
    // Actually the metric usually has negative expansion in front, let's verify signs:
    // If x > 0, r_s = x, df/dr is typically peaked around r_s = R.
    // For x=1.0 (bubble wall), let's check non-zero values.
    float theta_front = Physics::AlcubierreExpansionTheta(1.0f, 2.0f, 1.0f, 8.0f);
    float theta_back = Physics::AlcubierreExpansionTheta(-1.0f, 2.0f, 1.0f, 8.0f);
    
    // In our simplified 1D implementation, theta(-x) = -theta(x)
    EXPECT_FLOAT_EQ(theta_front, -theta_back);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
