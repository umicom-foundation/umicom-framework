#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/local_models/sdk_consumer/CMakeLists.txt
#
# PURPOSE:
#   Verify that an installed Framework SDK exports the native model service and its transitive dependencies.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.24)
project(UmicomModelConsumer LANGUAGES C)
find_package(UmicomFramework CONFIG REQUIRED)
add_executable(umicom-model-consumer main.c)
target_link_libraries(umicom-model-consumer PRIVATE Umicom::local_models)
set_target_properties(umicom-model-consumer PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
enable_testing()
add_test(NAME local_models.sdk_catalogue COMMAND umicom-model-consumer)
