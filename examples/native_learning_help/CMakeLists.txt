#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/native_learning_help/CMakeLists.txt
#
# PURPOSE:
#   Build an interactive local Help example against one explicit installed SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeLearningHelp LANGUAGES C)
if(NOT DEFINED UMICOM_EXPECTED_SDK_PREFIX OR NOT IS_ABSOLUTE "${UMICOM_EXPECTED_SDK_PREFIX}" OR
   NOT IS_DIRECTORY "${UMICOM_EXPECTED_SDK_PREFIX}")
    message(FATAL_ERROR "Supply UMICOM_EXPECTED_SDK_PREFIX as the installed SDK absolute directory")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS learning_catalogue_gtk4
    PATHS "${UMICOM_EXPECTED_SDK_PREFIX}" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
file(REAL_PATH "${UMICOM_EXPECTED_SDK_PREFIX}" _sdk_prefix)
file(REAL_PATH "${UmicomFramework_CONFIG}" _sdk_configuration)
cmake_path(IS_PREFIX _sdk_prefix "${_sdk_configuration}" NORMALIZE _inside)
if(NOT _inside)
    message(FATAL_ERROR "The selected package is outside the requested SDK prefix")
endif()
umicom_require_sdk_environment(2)
umicom_require_installed_sdk_layout("${UMICOM_EXPECTED_SDK_PREFIX}" _headers Umicom::learning_catalogue_gtk4)
add_executable(umicom-native-learning-help main.c)
target_link_libraries(umicom-native-learning-help PRIVATE Umicom::learning_catalogue_gtk4)
set_target_properties(umicom-native-learning-help PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MSVC)
    target_compile_options(umicom-native-learning-help PRIVATE /W4)
else()
    target_compile_options(umicom-native-learning-help PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

# This example is interactive. Its build does not claim a GUI acceptance pass.
# Configuration stages an explicit local pack with hashes of the copied bytes.
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pack")
configure_file(guide.html "${CMAKE_CURRENT_BINARY_DIR}/pack/guide.html" COPYONLY)
configure_file(main.c "${CMAKE_CURRENT_BINARY_DIR}/pack/main.c" COPYONLY)
file(SHA256 "${CMAKE_CURRENT_BINARY_DIR}/pack/guide.html" _guide_hash)
file(SHA256 "${CMAKE_CURRENT_BINARY_DIR}/pack/main.c" _source_hash)
configure_file(learning-pack.json.in "${CMAKE_CURRENT_BINARY_DIR}/pack/learning-pack.json" @ONLY)
