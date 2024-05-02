#ifndef TDOA_HPP
#define TDOA_HPP

#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include <ceres/ceres.h>

const double D = 6.0;

using namespace ceres;

struct Objective {
    Objective(double delta_r12, double delta_r13,
              const std::vector<std::vector<double>>& Rx)
            : delta_r12(delta_r12), delta_r13(delta_r13), Rx(Rx) {}

    template <typename T>
    bool operator()(const T* const xy, T* residual) const {
        residual[0] = ceres::sqrt((T(Rx[1][0]) - xy[0]) * (T(Rx[1][0]) - xy[0]) +
                                  (T(Rx[1][1]) - xy[1]) * (T(Rx[1][1]) - xy[1])) -
                      ceres::sqrt((T(Rx[0][0]) - xy[0]) * (T(Rx[0][0]) - xy[0]) +
                                  (T(Rx[0][1]) - xy[1]) * (T(Rx[0][1]) - xy[1])) -
                      T(delta_r12);
        residual[1] = ceres::sqrt((T(Rx[2][0]) - xy[0]) * (T(Rx[2][0]) - xy[0]) +
                                  (T(Rx[2][1]) - xy[1]) * (T(Rx[2][1]) - xy[1])) -
                      ceres::sqrt((T(Rx[0][0]) - xy[0]) * (T(Rx[0][0]) - xy[0]) +
                                  (T(Rx[0][1]) - xy[1]) * (T(Rx[0][1]) - xy[1])) -
                      T(delta_r13);
        return true;
    }

private:
    double delta_r12;
    double delta_r13;
    const std::vector<std::vector<double>>& Rx;
};

std::vector<double> calculate_distance(double delay_12, double delay_13);
std::vector<std::vector<double>> tdoa_calculation(double delta_r12, double delta_r13, std::vector<std::vector<double>> Rx);

#endif // TDOA_HPP
