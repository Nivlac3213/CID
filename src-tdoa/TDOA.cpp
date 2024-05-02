// cd to build dir in repo: cmake --build .

#include <iostream>
#include <cmath> // for std::abs
#include <vector>
#include <random> // for random number generation
//#include "ceres/ceres.h"
#include <ceres/ceres.h>
#include "TDOA.hpp"


//const double D = 10.0;

//g++ -I/usr/local/include/eigen3/ TDOA.cpp

using namespace ceres;

// Define the objective function
// Vector Function - Describes system of equations for TDOA
//struct Objective {
//    Objective(double delta_r12, double delta_r13,
//        const std::vector<std::vector<double>>& Rx)
//        : delta_r12(delta_r12), delta_r13(delta_r13), Rx(Rx) {}
//
//    template <typename T>
//
//    // Operator - Minimize Residual Using Nonlinear Least Squares
//    // We do not need three delta times to calculate the TDOA function
//    bool operator()(const T* const xy, T* residual) const {
//        residual[0] = ceres::sqrt((T(Rx[1][0]) - xy[0]) * (T(Rx[1][0]) - xy[0]) +
//            (T(Rx[1][1]) - xy[1]) * (T(Rx[1][1]) - xy[1])) -
//            ceres::sqrt((T(Rx[0][0]) - xy[0]) * (T(Rx[0][0]) - xy[0]) +
//                (T(Rx[0][1]) - xy[1]) * (T(Rx[0][1]) - xy[1])) -
//            T(delta_r12);
//        residual[1] = ceres::sqrt((T(Rx[2][0]) - xy[0]) * (T(Rx[2][0]) - xy[0]) +
//            (T(Rx[2][1]) - xy[1]) * (T(Rx[2][1]) - xy[1])) -
//            ceres::sqrt((T(Rx[0][0]) - xy[0]) * (T(Rx[0][0]) - xy[0]) +
//                (T(Rx[0][1]) - xy[1]) * (T(Rx[0][1]) - xy[1])) -
//            T(delta_r13);
//        return true;
//    }
//
//// Delta's are the distance difference using the time difference
//// Time difference comes from cross correlation
//// Delta's are used in the TDOA calculation
//private:
//    double delta_r12;
//    double delta_r13;
//    const std::vector<std::vector<double>>& Rx;
//};

// Calculates the Distance using Time Delays from Cross Correlation
std::vector<double> calculate_distance(double delay_12, double delay_13) {
//std::vector<std::vector<double>> calculate_distance(double delay_12, double delay_13) {
    
    // Constants
    const double c = 299792458.0; // speed of light in m/s

    // Calculating Distance 
    double delta_r12 = c * delay_12;
    double delta_r13 = c * delay_13;

    std::vector<double> distances = {delta_r12, delta_r13};
    return distances;
}

// Calculates X,Y from Delta Distances Between Receivers 
std::vector<std::vector<double>> tdoa_calculation(double delta_r12, double delta_r13, std::vector<std::vector<double>> Rx) {

//struct Objective {
//    Objective(double delta_r12, double delta_r13,
//        const std::vector<std::vector<double>>& Rx)
//        : delta_r12(delta_r12), delta_r13(delta_r13), Rx(Rx) {}
//
//    template <typename T>
//
//    // Operator - Minimize Residual Using Nonlinear Least Squares
//    // We do not need three delta times to calculate the TDOA function
//    bool operator()(const T* const xy, T* residual) const {
//        residual[0] = ceres::sqrt((T(Rx[1][0]) - xy[0]) * (T(Rx[1][0]) - xy[0]) +
//            (T(Rx[1][1]) - xy[1]) * (T(Rx[1][1]) - xy[1])) -
//            ceres::sqrt((T(Rx[0][0]) - xy[0]) * (T(Rx[0][0]) - xy[0]) +
//                (T(Rx[0][1]) - xy[1]) * (T(Rx[0][1]) - xy[1])) -
//            T(delta_r12);
//        residual[1] = ceres::sqrt((T(Rx[2][0]) - xy[0]) * (T(Rx[2][0]) - xy[0]) +
//            (T(Rx[2][1]) - xy[1]) * (T(Rx[2][1]) - xy[1])) -
//            ceres::sqrt((T(Rx[0][0]) - xy[0]) * (T(Rx[0][0]) - xy[0]) +
//                (T(Rx[0][1]) - xy[1]) * (T(Rx[0][1]) - xy[1])) -
//            T(delta_r13);
//        return true;
//    }
//
//// Delta's are the distance difference using the time difference
//// Time difference comes from cross correlation
//// Delta's are used in the TDOA calculation
//private:
//    double delta_r12;
//    double delta_r13;
//    const std::vector<std::vector<double>>& Rx;
//};

    // Initial guess for transmitter position
    // double x[2] = { D * static_cast<double>(std::rand()) / RAND_MAX, D * static_cast<double>(std::rand()) / RAND_MAX };
    double x[2] = { D/2, D/2 };

    // Create the problem
    ceres::Problem problem;

    // Add the cost function
    ceres::CostFunction* cost_function =
        new ceres::AutoDiffCostFunction<Objective, 2, 2>(
            new Objective(delta_r12, delta_r13, Rx));
    problem.AddResidualBlock(cost_function, nullptr, x);

    // Add constraints
    problem.SetParameterLowerBound(x, 0, 0); // Lower bound for x coordinate

    // Solve the problem
    ceres::Solver::Options options;
    options.linear_solver_type = ceres::DENSE_QR;
    options.minimizer_progress_to_stdout = true;
    ceres::Solver::Summary summary;
    ceres::Solve(options, &problem, &summary);

    // Output result
    //std::cout << summary.BriefReport() << "\n";
    std::cout << "Transmitter position: x = " << x[0] << ", y = " << x[1] << "\n";

//    std::vector<std::vector<double>> position = {x[0], x[1]};
    std::vector<std::vector<double>> position = {{x[0], x[1]}};
    return position;
}

//int main() {
//
//    //*************************************************************************************************//
//    // Initialization //
//    //*************************************************************************************************//
//
//    // Constants
//    //const double D = 10.0; // dimensions of a room, represents 10x10 meter room
//    const int nRx = 3; // Number of receivers
//    const int nTx = 1; // Number of emitters
//    const int dim = 2; // Number of dimensions for the problem
//
//    // NEEDED FOR SIMULATION
//    // Random number generator setup
//    /* std::random_device rd;
//    std::mt19937 gen(rd());
//    std::uniform_real_distribution<> dis(0, D); // Uniform distribution between 0 and D
//    */
//
//    // NEED FOR SIMULATION
//    // Transmitter positions
//    /* std::vector<std::vector<double>> Tx(nTx, std::vector<double>(dim));
//    Tx[0][0] = std::abs(dis(gen)); // Random transmitter positions
//    Tx[0][1] = std::abs(dis(gen)); // Random transmitter positions
//    */
//
//    // UPDATE, once receiver positions are defined
//    // Receiver positions, in m (x, y)
//    std::vector<std::vector<double>> Rx = { {0, 0}, {D, 0}, {0.25 * D, D} };
//
//    //*************************************************************************************************//
//    // Delta calc //
//    //*************************************************************************************************//
//
//    /* // Constants
//    const double c = 299792458.0; // speed of light in m/s
//
//    // Calculating Distance
//    double delta_r12 = c * delay_12;
//    double delta_r13 = c * delay_13;
//    */
//
//    // NEEDED FOR SIMULATION
//    // Compute distances
//    /* double distance1 = std::sqrt(std::pow(Rx[0][0] - Tx[0][0], 2) + std::pow(Rx[0][1] - Tx[0][1], 2));
//    double distance2 = std::sqrt(std::pow(Rx[1][0] - Tx[0][0], 2) + std::pow(Rx[1][1] - Tx[0][1], 2));
//    double distance3 = std::sqrt(std::pow(Rx[2][0] - Tx[0][0], 2) + std::pow(Rx[2][1] - Tx[0][1], 2));
//
//    // Compute distance differences
//    double delta_r12 = distance2 - distance1;
//    double delta_r13 = distance3 - distance1;
//    double delta_r23 = distance3 - distance2;
//
//    std::cout << "Transmitter positions:\n";
//    std::cout << " (" << Tx[0][0] << ", " << Tx[0][1] << ")\n";
//    */
//
//    std::cout << "\nReceiver positions:\n";
//    for (int i = 0; i < nRx; ++i) {
//        std::cout << "Rx[" << i << "]: (" << Rx[i][0] << ", " << Rx[i][1] << ")\n";
//    }
//
////    double delay_12 = 12;
////    double delay_13 = 13;
//
//
//    // Calculate Distance using Cross Correlation Delays
//    //std::vector<double> distances = calculate_distance(delay_12, delay_13); //td::vector<std::vector<double>>
//
//    //std::vector<std::vector<double>> position = tdoa_calculation(distances[0], distances[1], Rx);
//
//    /*// Initial guess for transmitter position
//    // double x[2] = { D * static_cast<double>(std::rand()) / RAND_MAX, D * static_cast<double>(std::rand()) / RAND_MAX };
//    double x[2] = { D/2, D/2 };
//
//    // Create the problem
//    ceres::Problem problem;
//
//    // Add the cost function
//    ceres::CostFunction* cost_function =
//        new ceres::AutoDiffCostFunction<Objective, 2, 2>(
//            new Objective(delta_r12, delta_r13, Rx));
//    problem.AddResidualBlock(cost_function, nullptr, x);
//
//    // Add constraints
//    problem.SetParameterLowerBound(x, 0, 0); // Lower bound for x coordinate
//
//    // Solve the problem
//    ceres::Solver::Options options;
//    options.linear_solver_type = ceres::DENSE_QR;
//    options.minimizer_progress_to_stdout = true;
//    ceres::Solver::Summary summary;
//    ceres::Solve(options, &problem, &summary);
//
//    // Output result
//    std::cout << summary.BriefReport() << "\n";
//    std::cout << "Transmitter position: x = " << x[0] << ", y = " << x[1] << "\n";
//    */
//
//    return 0;
//}
