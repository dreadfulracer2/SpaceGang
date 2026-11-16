
#include "RocketLaunchSimulator.h"

#include <array>

namespace
{
    struct ComponentStats
    {
        int price;
        int mass;
        int thrust;
        int crewCapacity;
        double velocity;
    };

    constexpr std::array<ComponentStats, 3> cabins = { {
        {5, 1000, 0, 0, 0.0},
        {18, 4000, 0, 2, 0.0},
        {45, 9000, 0, 6, 0.0}
    } };

    constexpr std::array<ComponentStats, 3> boosters1 = { {
        {15, 30000, 350, 0, 0.0},
        {28, 55000, 850, 0, 0.0},
        {42, 85000, 1500, 0, 0.0}
    } };

    constexpr std::array<ComponentStats, 3> boosters2 = { {
        {12, 8000, 80, 0, 2.0},
        {24, 14000, 180, 0, 3.5},
        {38, 22000, 300, 0, 5.0}
    } };

    constexpr double GRAVITY = 9.81;
    constexpr double MIN_THRUST_TO_WEIGHT = 1.2;
    constexpr double REQUIRED_VELOCITY = 3.5;
}

LaunchReport RocketLaunchSimulator::simulate(
    const std::array<std::uint8_t, 3>& winners) const
{
    LaunchReport report;

    // Validar indices antes de acceder a los arreglos.
    for (auto option : winners)
    {
        if (option > 2)
            return report;
    }

    const auto& cabin = cabins[winners[0]];
    const auto& booster1 = boosters1[winners[1]];
    const auto& booster2 = boosters2[winners[2]];

    report.totalPrice =
        cabin.price + booster1.price + booster2.price;

    report.totalMass =
        cabin.mass + booster1.mass + booster2.mass;

    report.crewCapacity = cabin.crewCapacity;
    report.secondStageVelocity = booster2.velocity;

    // Convertir kN a N para calcular la relacion.
    report.thrustToWeight =
        (booster1.thrust * 1000.0) /
        (report.totalMass * GRAVITY);

    if (report.crewCapacity == 0)
    {
        report.result = LaunchResult::NoCrew;
    }
    else if (report.totalPrice > BUDGET)
    {
        report.result = LaunchResult::OverBudget;
    }
    else if (report.thrustToWeight < MIN_THRUST_TO_WEIGHT)
    {
        report.result = LaunchResult::InsufficientThrust;
    }
    else if (report.secondStageVelocity < REQUIRED_VELOCITY)
    {
        report.result = LaunchResult::InsufficientSecondStage;
    }
    else
    {
        report.result = LaunchResult::Success;
    }

    return report;
}
