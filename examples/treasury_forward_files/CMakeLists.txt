# Umicom Framework
# File: examples/treasury_forward_files/CMakeLists.txt
# PURPOSE: Demonstrate the installed optional treasury file adapter.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomTreasuryForwardFiles LANGUAGES C)
if(NOT IS_ABSOLUTE "${UMICOM_SDK_PREFIX}" OR NOT IS_DIRECTORY "${UMICOM_SDK_PREFIX}")
    message(FATAL_ERROR "Set UMICOM_SDK_PREFIX to the intended installed Framework SDK.")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS treasury_files
    PATHS "${UMICOM_SDK_PREFIX}" NO_DEFAULT_PATH)
# The original draft call is retained for review. The SDK validator requires
# its selected prefix and each exported component whose closure is exercised.
if(FALSE)
    umicom_require_installed_sdk()
endif()
umicom_require_installed_sdk("${UMICOM_SDK_PREFIX}" Umicom::treasury_files Umicom::finance Umicom::platform)
add_executable(treasury-forward-files main.c)
target_link_libraries(treasury-forward-files PRIVATE Umicom::treasury_files)
set_target_properties(treasury-forward-files PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)

if(MSVC)
    target_compile_options(treasury-forward-files PRIVATE /W4)
else()
    target_compile_options(treasury-forward-files PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
