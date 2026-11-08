#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomProfileName.cmake
#
# PURPOSE:
#   Compose the portable profile-name C library without native account dependencies.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(GNUInstallDirs)
# This composition file can be included by the full SDK or a cross-target lab.
# It never configures the Framework root or selects a compiler for its caller.
# Installation belongs to the full SDK; a source consumer owns its own package.
set(_umi_profile_name_root "${CMAKE_CURRENT_LIST_DIR}/..")
if(TARGET umicom_profile_name OR TARGET Umicom::profile_name)
    message(FATAL_ERROR "The profile-name target is already defined; use one Framework source or SDK")
endif()
add_library(umicom_profile_name STATIC
    "${_umi_profile_name_root}/src/security/profile_name.c")
add_library(Umicom::profile_name ALIAS umicom_profile_name)
set_target_properties(umicom_profile_name PROPERTIES
    EXPORT_NAME profile_name
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    POSITION_INDEPENDENT_CODE YES)
target_include_directories(umicom_profile_name PUBLIC
    $<BUILD_INTERFACE:${_umi_profile_name_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_profile_name)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_profile_name)
endif()
unset(_umi_profile_name_root)
