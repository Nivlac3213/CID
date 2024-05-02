
# Add your source files
add_executable(serverApp main.cpp StreamServerCore.cpp SignalDataService.cpp RxwFunctions.cpp SignalProcessor.cpp TDOA.cpp CrossCorrelation.cpp)

# Link pthread library
find_package(Threads REQUIRED)

target_link_libraries(serverApp PRIVATE Threads::Threads ${CERES_LIBRARIES} sqlite3)

# Set C++ standard
set_target_properties(serverApp PROPERTIES CXX_STANDARD 14)







