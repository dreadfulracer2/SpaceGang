
#pragma once

#include <array>
#include <cstdint>

enum class LaunchResult : std::uint8_t
{
    NoCrew = 0,
    OverBudget = 1,
    InsufficientThrust = 2,
    InsufficientSecondStage = 3,
    Success = 4
};

struct LaunchReport
{
    LaunchResult result = LaunchResult::NoCrew;

    int totalPrice = 0;
    int totalMass = 0;
    int crewCapacity = 0;

    double thrustToWeight = 0.0;
    double secondStageVelocity = 0.0;
};

class RocketLaunchSimulator
{
public:
    static constexpr int BUDGET = 100;

    LaunchReport simulate(
        const std::array<std::uint8_t, 3>& winners
    ) const;
};
