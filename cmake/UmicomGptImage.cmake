#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomGptImage.cmake
#
# PURPOSE:
#   Compose reusable GPT inspection without device or GUI dependencies.
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
set(_umi_gpt_root "${CMAKE_CURRENT_LIST_DIR}/..")
add_library(umicom_gpt_image STATIC "${_umi_gpt_root}/src/image_inspection/gpt.c")
add_library(Umicom::gpt_image ALIAS umicom_gpt_image)
set_target_properties(umicom_gpt_image PROPERTIES EXPORT_NAME gpt_image
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_gpt_image PUBLIC
    $<BUILD_INTERFACE:${_umi_gpt_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_gpt_image)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_gpt_image)
endif()
install(TARGETS umicom_gpt_image EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
install(FILES "${_umi_gpt_root}/include/umicom/image_inspection/gpt.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/image_inspection COMPONENT Framework)
umicom_register_sdk_libraries(umicom_gpt_image)
if(BUILD_TESTING)
    add_subdirectory("${_umi_gpt_root}/tests/gpt_inspection"
        "${CMAKE_CURRENT_BINARY_DIR}/gpt-image-tests")
endif()
install(FILES "${_umi_gpt_root}/docs/learning/inspect-gpt-partition-tables.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
unset(_umi_gpt_root)
