//
// Created by fedor on 31.10.2024.
//
#include "../include/funcs.h"

long double get_viscosity(long double temperature_of_gas, long double S, long double T0, long double v0)
{
    return v0 * std::pow((temperature_of_gas / T0), 1.5) * ((T0 + S) / (temperature_of_gas + S));
}

long double get_velocity(long double eps, long double temperature_of_gas, long double radius,
                         long double density_gas, long double g, long double S, long double T0, long double v0)
{
    long double tmp = get_viscosity(temperature_of_gas, S, T0, v0);
    if (std::abs(tmp) < eps)
        throw std::runtime_error("division by zero!");
    return (2.0 / 9.0) * (density_gas * radius * radius * g / tmp);
}

long double get_Pr(long double temperature_of_gas, long double Cp_air, long double k, long double S, long double T0, long double v0)
{
    return Cp_air * get_viscosity(temperature_of_gas, S, T0, v0)/ k;
}

long double get_Re(long double eps, long double temperature_of_gas, long double radius,
                   long double density_air,
                   long double S, long double T0, long double v0, long double g)
{
    auto temp = get_viscosity(temperature_of_gas, S, T0, v0);
    if (std::abs(temp) < eps)
        throw std::runtime_error("division by zero!");
    return density_air * 2 * radius * get_velocity(eps, temperature_of_gas, radius, density_air, g, S, T0, v0) / temp;
}

long double get_h(long double eps, long double k, long double radius,
                  long double temperature_gas, long double density_air,
                  long double S, long double T0, long double v0, long double Cp_air, long double g)
{
    long double tmp = 2 * radius;
    if (std::abs(tmp) < eps)
        throw std::runtime_error("division by zero!");

    return 0.023 * std::pow(get_Re(eps, temperature_gas, radius, density_air, S, T0, v0, g), 0.8) *
           std::pow(get_Pr(temperature_gas, Cp_air, k, S, T0, v0), 0.4) * k / tmp;
}

long double func_major(long double eps, long double temperature_gas, long double radius,
                       long double current_temperature, long double density_air, long double density_material, long double Cp_Fe,
                       long double Cp_air,
                       long double k, long double S, long double T0, long double v0, long double g)
{
    long double tmp = density_material * radius * Cp_Fe;
    if (std::abs(tmp) < eps)
        throw std::runtime_error("division by zero!");

    return 3 * get_h(eps, k, radius, temperature_gas, density_air, S, T0, v0, Cp_air, g) *
           (temperature_gas - current_temperature) / tmp;
}

long double runge_kutta_4th_order(long double current_temperature, long double step,
                                  long double eps, long double temperature_gas,
                                  long double radius, long double density_air,
                                  long double density_material, long double Cp_Fe,
                                  long double Cp_air, long double k, long double S,
                                  long double T0, long double v0, long double g)
{

    long double k1 = step * func_major(eps, temperature_gas, radius, current_temperature,
                                       density_air, density_material, Cp_Fe, Cp_air,
                                       k, S, T0, v0, g);
    long double k2 = step * func_major(eps, temperature_gas, radius, current_temperature + 0.5 * k1,
                                       density_air, density_material, Cp_Fe, Cp_air,
                                       k, S, T0, v0, g);
    long double k3 = step * func_major(eps, temperature_gas, radius, current_temperature + 0.5 * k2,
                                       density_air, density_material, Cp_Fe, Cp_air,
                                       k, S, T0, v0, g);
    long double k4 = step * func_major(eps, temperature_gas, radius, current_temperature + k3,
                                       density_air, density_material, Cp_Fe, Cp_air,
                                       k, S, T0, v0, g);

    return current_temperature + (1.0 / 6.0) * (k1 + 2 * k2 + 2 * k3 + k4);
}


void runge_kutta_4_distance(long double &velocity, long double &distance, long double step, long double g)
{

    long double k1_v = g;
    long double k2_v = g;
    long double k3_v = g;
    long double k4_v = g;


    velocity += (step / 6.0) * (k1_v + 2.0 * k2_v + 2.0 * k3_v + k4_v);


    long double k1_x = velocity;
    long double k2_x = velocity + 0.5 * step * k1_v;
    long double k3_x = velocity + 0.5 * step * k2_v;
    long double k4_x = velocity + step * k3_v;


    distance += (step / 6.0) * (k1_x + 2.0 * k2_x + 2.0 * k3_x + k4_x);
}
