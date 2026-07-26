cmake_minimum_required(VERSION 3.16)

project(bond_pricing_lab)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_library(QUANTLIB_LIBRARY QuantLib REQUIRED)

add_executable(bond_pricing_lab src/main.cpp)

target_link_libraries(bond_pricing_lab PRIVATE ${QUANTLIB_LIBRARY})
