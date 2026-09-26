# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomDesktopSystemChecks LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include(GNUInstallDirs)
get_filename_component(UMICOM_DESKTOP_SYSTEM_SOURCE "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
option(UMICOM_DESKTOP_SYSTEM_STRICT "Treat warnings as errors in this focused project" ON)
option(UMICOM_DESKTOP_SYSTEM_SANITIZE "Address and undefined-behaviour sanitizers" OFF)
function(umicom_apply_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        if(UMICOM_DESKTOP_SYSTEM_STRICT)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()
endfunction()
function(umicom_apply_sanitizers target)
    if(UMICOM_DESKTOP_SYSTEM_SANITIZE AND NOT MSVC)
        target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(${target} PRIVATE -fsanitize=address,undefined)
    endif()
endfunction()
# Standalone checks compile the ACTUAL boot-report reader. This deliberately
# small platform target is not the full Framework platform or an application.
if(NOT TARGET Umicom::platform)
    add_library(umicom_desktop_platform_subset STATIC
        "${UMICOM_DESKTOP_SYSTEM_SOURCE}/src/platform/boot_report.c")
    add_library(Umicom::platform ALIAS umicom_desktop_platform_subset)
    set_target_properties(umicom_desktop_platform_subset PROPERTIES EXPORT_NAME platform)
    target_include_directories(umicom_desktop_platform_subset PUBLIC
        $<BUILD_INTERFACE:${UMICOM_DESKTOP_SYSTEM_SOURCE}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
    umicom_apply_warnings(umicom_desktop_platform_subset)
    umicom_apply_sanitizers(umicom_desktop_platform_subset)
    install(TARGETS umicom_desktop_platform_subset EXPORT UmicomFrameworkTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(FILES "${UMICOM_DESKTOP_SYSTEM_SOURCE}/include/umicom/base/status.h"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/base)
    install(FILES "${UMICOM_DESKTOP_SYSTEM_SOURCE}/include/umicom/platform/boot_report.h"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/platform)
endif()
include("${UMICOM_DESKTOP_SYSTEM_SOURCE}/cmake/UmicomDesktopSystem.cmake")
install(EXPORT UmicomFrameworkTargets FILE UmicomDesktopSystemConfig.cmake
    NAMESPACE Umicom:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/UmicomDesktopSystem)
