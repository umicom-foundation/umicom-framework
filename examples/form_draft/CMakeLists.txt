# Umicom Framework
# File: examples/form_draft/CMakeLists.txt
#
# PURPOSE:
# Build an independent offline atomic form draft example from a selected installed Framework SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT


cmake_minimum_required(VERSION 3.24)
project(UmicomFormDraftSdkExample LANGUAGES C)
set(UMICOM_SDK_ROOT "" CACHE PATH "Selected installed Framework SDK prefix")
if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
    message(FATAL_ERROR "Set UMICOM_SDK_ROOT to an absolute existing installed Framework SDK prefix")
endif()
find_package(UmicomFramework 0.9 CONFIG REQUIRED COMPONENTS application
    PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
if(NOT COMMAND umicom_require_installed_sdk OR NOT COMMAND umicom_require_sdk_environment OR
   NOT COMMAND umicom_apply_application_branding)
    message(FATAL_ERROR "FormDraft example requires current installed SDK identity and branding helpers")
endif()
# Validate the selected package origin and ABI through its shared services.
# The consumer includes public headers and never compiles Framework sources.
umicom_require_installed_sdk("${UMICOM_SDK_ROOT}" Umicom::application)
umicom_require_sdk_environment(2)
add_executable(umicom-sdk-form-draft main.c)
target_link_libraries(umicom-sdk-form-draft PRIVATE Umicom::application)
set_target_properties(umicom-sdk-form-draft PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
umicom_apply_application_branding(TARGET umicom-sdk-form-draft
    PRODUCT_NAME "Umicom FormDraft SDK Example" INTERNAL_NAME "umicom-sdk-form-draft"
    APPLICATION_ID "org.umicom.sdk.form-draft" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
include(CTest)
if(BUILD_TESTING)
    add_test(NAME sdk.form-draft.isolated-commit COMMAND "$<TARGET_FILE:umicom-sdk-form-draft>")
    set_tests_properties(sdk.form-draft.isolated-commit PROPERTIES
        TIMEOUT 20 LABELS "sdk;forms;example")
endif()

# A second thin consumer exercises refresh/conflict semantics using the same
# selected SDK. The original isolated-commit example remains unchanged.
add_executable(umicom-sdk-form-reconcile reconcile.c)
target_link_libraries(umicom-sdk-form-reconcile PRIVATE Umicom::application)
set_target_properties(umicom-sdk-form-reconcile PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
umicom_apply_application_branding(TARGET umicom-sdk-form-reconcile
    PRODUCT_NAME "Umicom Draft Refresh SDK Example" INTERNAL_NAME "umicom-sdk-form-reconcile"
    APPLICATION_ID "org.umicom.sdk.form-reconcile" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
if(BUILD_TESTING)
    add_test(NAME sdk.form-draft.refresh-conflict COMMAND "$<TARGET_FILE:umicom-sdk-form-reconcile>")
    set_tests_properties(sdk.form-draft.refresh-conflict PROPERTIES TIMEOUT 20 LABELS "sdk;forms;example")
endif()
