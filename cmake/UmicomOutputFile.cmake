#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomOutputFile.cmake
#
# PURPOSE:
#   Compose exclusive raw output capture independently of higher-level platform services.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
include(GNUInstallDirs)
set(_umi_output_root "${CMAKE_CURRENT_LIST_DIR}/..")
# One implementation owns exclusive file creation for both protocol channels and
# the general platform library. No GUI, financial or thread service is required.
add_library(umicom_output_file STATIC "${_umi_output_root}/src/platform/output_file.c")
add_library(Umicom::output_file ALIAS umicom_output_file)
set_target_properties(umicom_output_file PROPERTIES EXPORT_NAME output_file
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_output_file PUBLIC
    $<BUILD_INTERFACE:${_umi_output_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_output_file)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_output_file)
endif()
install(TARGETS umicom_output_file EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
umicom_register_sdk_libraries(umicom_output_file)
install(FILES "${_umi_output_root}/include/umicom/platform/output_file.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/platform COMPONENT Framework)
unset(_umi_output_root)
