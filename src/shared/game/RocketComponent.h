
#pragma once

#include <cstdint>
#include <string>

enum class ComponentType : std::uint8_t
{
    CrewModule = 0,
    Booster1 = 1,
    Booster2 = 2
};

struct RocketComponent
{
    std::uint32_t id = 0;

    ComponentType type = ComponentType::CrewModule;

    std::string name;

    // Propiedades fisicas y economicas
    double price = 0.0;
    double mass = 0.0;
    double thrust = 0.0;
    double fuelConsumption = 0.0;

    // Propiedades de compatibilidad
    double diameter = 0.0;
    double height = 0.0;

    // Capacidad de tripulantes
    std::uint32_t crewCapacity = 0;

    // Ruta del sprite
    std::string spritePath;
};

