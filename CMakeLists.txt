cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Find Ceres Solver package
find_package(Ceres REQUIRED)

# Include Ceres headers
include_directories(${CERES_INCLUDE_DIRS})

# Add the source directory
add_subdirectory(src)
