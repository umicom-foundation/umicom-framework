#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomIsoImage.cmake
#
# PURPOSE:
#   Compose the shared ISO structural inspector and its owner-run checks.
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
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
set(_umi_iso_root "${CMAKE_CURRENT_LIST_DIR}/..")
# No setup/media dependency: both workflows consume this read-only parser.
add_library(umicom_iso_image STATIC "${_umi_iso_root}/src/image_inspection/iso.c")
add_library(Umicom::iso_image ALIAS umicom_iso_image)
set_target_properties(umicom_iso_image PROPERTIES EXPORT_NAME iso_image
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_iso_image PUBLIC
    $<BUILD_INTERFACE:${_umi_iso_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_iso_image)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_iso_image)
endif()
install(TARGETS umicom_iso_image EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
umicom_register_sdk_libraries(umicom_iso_image)
install(DIRECTORY "${_umi_iso_root}/include/umicom/image_inspection"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom COMPONENT Framework)
if(BUILD_TESTING)
    add_subdirectory("${_umi_iso_root}/tests/image_inspection"
        "${CMAKE_CURRENT_BINARY_DIR}/iso-image-tests")
endif()
unset(_umi_iso_root)
