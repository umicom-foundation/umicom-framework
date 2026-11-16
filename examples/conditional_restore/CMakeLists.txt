# Umicom Framework
# File: examples/conditional_restore/CMakeLists.txt
#
# PURPOSE:
#   Build a reviewed-restore consumer from an explicit installed SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomConditionalRestore LANGUAGES C)
set(UMICOM_SDK_ROOT "" CACHE PATH "Selected installed Framework SDK")
if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
    message(FATAL_ERROR "Choose an absolute existing UMICOM_SDK_ROOT")
endif()
find_package(UmicomFramework 0.9 CONFIG REQUIRED COMPONENTS web
    PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
umicom_require_installed_sdk("${UMICOM_SDK_ROOT}" Umicom::web)
umicom_require_sdk_environment(2)
add_executable(umicom-conditional-restore main.c)
target_link_libraries(umicom-conditional-restore PRIVATE Umicom::web)
set_target_properties(umicom-conditional-restore PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umicom-conditional-restore)
umicom_apply_sanitizers(umicom-conditional-restore)
include(CTest)
if(BUILD_TESTING)
    add_test(NAME sdk.conditional_restore COMMAND umicom-conditional-restore)
endif()
