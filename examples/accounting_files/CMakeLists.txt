# Umicom Framework
# File: examples/accounting_files/CMakeLists.txt
# PURPOSE: Exercise accounting persistence through the requested installed SDK only.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomAccountingFiles LANGUAGES C)
if(NOT IS_ABSOLUTE "${UMICOM_SDK_PREFIX}" OR NOT IS_DIRECTORY "${UMICOM_SDK_PREFIX}")
    message(FATAL_ERROR "Set UMICOM_SDK_PREFIX to an absolute installed Framework prefix")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS treasury_files
    PATHS "${UMICOM_SDK_PREFIX}" NO_DEFAULT_PATH)
umicom_require_installed_sdk("${UMICOM_SDK_PREFIX}"
    Umicom::treasury_files Umicom::finance Umicom::platform Umicom::base)
add_executable(accounting-files main.c)
target_link_libraries(accounting-files PRIVATE Umicom::treasury_files)
set_target_properties(accounting-files PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MSVC)
    target_compile_options(accounting-files PRIVATE /W4)
else()
    target_compile_options(accounting-files PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
# This example takes an explicit, new absolute output path. It never chooses a
# user's location or reuses an older archive during configuration or CTest.
