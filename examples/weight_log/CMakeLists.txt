#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/weight_log/CMakeLists.txt
#
# PURPOSE:
#   Build the small C client against an already installed Framework SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#---------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomWeightLogExample LANGUAGES C)
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS application)
add_executable(umicom-weight-log-example main.c)
target_link_libraries(umicom-weight-log-example PRIVATE Umicom::application)
set_target_properties(umicom-weight-log-example PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)

include(CTest)
if(BUILD_TESTING)
    add_test(NAME weight-log-example COMMAND umicom-weight-log-example)
endif()
