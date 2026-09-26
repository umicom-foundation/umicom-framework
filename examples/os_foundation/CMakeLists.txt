#-----------------------------------------------------------------------------
# Umicom Framework | Author: Sammy Hegab, Umicom Foundation | Licence: MIT
# A small build surface for the actual canonical source files, not a second
# implementation. Used to package the normal user-space probe independently
# of the Linux kernel and minimal recovery controller.
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomOsFoundationProbe LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
get_filename_component(UMICOM_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
option(UMICOM_OS_PROBE_STATIC "Link the guest probe statically" OFF)
if(NOT TARGET Umicom::data)
    add_library(umicom_probe_data STATIC "${UMICOM_ROOT}/src/data/data_server.c")
    target_include_directories(umicom_probe_data PUBLIC "${UMICOM_ROOT}/include")
    add_library(Umicom::data ALIAS umicom_probe_data)
endif()
add_library(umicom_probe_report STATIC "${UMICOM_ROOT}/src/platform/boot_report.c")
target_include_directories(umicom_probe_report PUBLIC "${UMICOM_ROOT}/include")
add_executable(umicom-framework-probe probe.c)
target_link_libraries(umicom-framework-probe PRIVATE Umicom::data umicom_probe_report)
if(UMICOM_OS_PROBE_STATIC AND NOT MSVC)
    target_link_options(umicom-framework-probe PRIVATE -static)
endif()
if(BUILD_TESTING)
    add_test(NAME framework.os_foundation.probe COMMAND umicom-framework-probe --self-test)
endif()
install(TARGETS umicom-framework-probe RUNTIME DESTINATION usr/libexec)
