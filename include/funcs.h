//
// Created by fedor on 31.10.2024.
//

#ifndef CWNUM_FUNCS_H
#define CWNUM_FUNCS_H

#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <functional>
//#include <algorithm>

long double get_viscosity(long double temperature_of_gas, long double S, long double T0, long double v0);
long double get_velocity(long double eps, long double temperature_of_gas, long double radius,
                         long double density_gas, long double g, long double S, long double T0, long double v0);

long double get_Pr(long double temperature_of_gas, long double Cp_air, long double k, long double S, long double T0, long double v0);

long double get_Re(long double eps, long double temperature_of_gas, long double radius,
                   long double density_air,
                   long double S, long double T0, long double v0, long double g);

long double get_h(long double eps, long double k, long double radius,
                  long double temperature_gas, long double density_air,
                  long double S, long double T0, long double v0, long double Cp_air, long double g);

long double func_major(long double eps, long double temperature_gas, long double radius,
                       long double current_temperature, long double density_air, long double density_material, long double Cp_Fe,
                       long double Cp_air,
                       long double k, long double S, long double T0, long double v0, long double g);

long double runge_kutta_4th_order(long double current_temperature, long double step,
                                  long double eps, long double temperature_gas,
                                  long double radius, long double density_air,
                                  long double density_material, long double Cp_Fe,
                                  long double Cp_air, long double k, long double S,
                                  long double T0, long double v0, long double g);

void runge_kutta_4_distance(long double &velocity, long double &distance, long double step, long double g);

#endif //CWNUM_FUNCS_H
