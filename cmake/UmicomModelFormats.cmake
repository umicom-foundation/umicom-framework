# Umicom Framework
# File: cmake/UmicomModelFormats.cmake
#
# PURPOSE:
# Export native model format inspection through the canonical protocol JSON owner and installed SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
include(GNUInstallDirs)
option(UMICOM_BUILD_MODEL_FORMATS "Build bounded native model metadata inspectors" ON)
if(NOT UMICOM_BUILD_MODEL_FORMATS)
    return()
endif()
if(NOT TARGET Umicom::protocol)
    message(STATUS "Native model format inspection disabled: canonical protocol target is unavailable")
    return()
endif()
if(TARGET Umicom::model_formats)
    return()
endif()
get_filename_component(_model_formats_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
add_library(umicom_model_formats STATIC "${_model_formats_root}/src/model_formats/safetensors.c")
target_sources(umicom_model_formats PRIVATE
    "${_model_formats_root}/src/model_formats/safetensors_file.c"
    "${_model_formats_root}/src/model_formats/header_internal.h")
add_library(Umicom::model_formats ALIAS umicom_model_formats)
set_target_properties(umicom_model_formats PROPERTIES EXPORT_NAME model_formats
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_model_formats PUBLIC
    $<BUILD_INTERFACE:${_model_formats_root}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_model_formats PUBLIC Umicom::protocol)
umicom_apply_warnings(umicom_model_formats)
umicom_apply_sanitizers(umicom_model_formats)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
install(TARGETS umicom_model_formats EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
umicom_register_sdk_libraries(umicom_model_formats)
install(FILES "${_model_formats_root}/include/umicom/model_formats/safetensors.h"
    "${_model_formats_root}/include/umicom/model_formats/safetensors_file.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/model_formats" COMPONENT Framework)
foreach(_component Framework FrameworkSDK)
    set(_selection EXCLUDE_FROM_ALL)
    if(_component STREQUAL "Framework")
        set(_selection)
    endif()
    install(DIRECTORY "${_model_formats_root}/examples/model_formats/"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/examples/model_formats"
        COMPONENT "${_component}" ${_selection})
endforeach()

if(BUILD_TESTING)
    add_executable(umicom-safetensors-header-test "${_model_formats_root}/tests/model_formats/test_safetensors.c")
    add_executable(umicom-safetensors-header-allocation-test "${_model_formats_root}/tests/model_formats/test_safetensors_allocation.c")
    foreach(_target umicom-safetensors-header-test umicom-safetensors-header-allocation-test)
        target_link_libraries(${_target} PRIVATE Umicom::model_formats)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_target} FILES
                "${_model_formats_root}/src/model_formats/safetensors.c"
                "${_model_formats_root}/src/model_formats/header_internal.h"
                "${_model_formats_root}/src/language_runtime/json_tree.c"
                "${_model_formats_root}/src/language_runtime/json_tree_compare.inc"
                "${_model_formats_root}/tests/model_formats/safetensors_fixture.h")
        endif()
    endforeach()
    foreach(_case IN ITEMS valid
            empty
            metadata-only
            scalar
            zero-dimension
            large-zero-dimension
            unordered
            empty-boundary
            space-padding
            tab-padding
            leading-space
            root-array
            bad-json
            duplicate-root
            escaped-root
            duplicate-metadata
            duplicate-dtype
            metadata-value
            metadata-array
            record-array
            missing-field
            unknown-field
            extra-field
            unknown-dtype
            sub-byte-dtype
            nonstring-dtype
            shape-not-array
            negative-dimension
            fractional-dimension
            string-dimension
            dimension-overflow
            product-overflow
            byte-overflow
            offset-count
            negative-offset
            reversed-offset
            span-mismatch
            out-of-buffer
            gap
            overlap
            trailing-data
            empty-payload-gap
            nul-text
            short-prefix
            short-header
            long-header
            missing-header
            file-bound
            prefix-exceeds-file
            null-prefix
            null-output
            cancel
            payload-ignored
            rank-bound
            maximum-name
            name-bound
            dtypes)
        set(_test "framework.model_formats.safetensors.${_case}")
        add_test(NAME "${_test}" COMMAND umicom-safetensors-header-test "${_case}")
        set_tests_properties("${_test}" PROPERTIES TIMEOUT 30
            LABELS "framework;model-formats;metadata;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${_test}" TARGETS umicom-safetensors-header-test)
        endif()
    endforeach()
    foreach(_case IN ITEMS allocation parser unique malformed)
        set(_test "framework.model_formats.safetensors_allocation.${_case}")
        add_test(NAME "${_test}" COMMAND umicom-safetensors-header-allocation-test "${_case}")
        set_tests_properties("${_test}" PROPERTIES TIMEOUT 30
            LABELS "framework;model-formats;metadata;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${_test}" TARGETS umicom-safetensors-header-allocation-test)
        endif()
    endforeach()
endif()

# File fixtures are created only by owner-run native tests in their isolated directories.
if(BUILD_TESTING)
    add_executable(umicom-safetensors-file-test "${_model_formats_root}/tests/model_formats/test_safetensors_file.c")
    add_executable(umicom-safetensors-file-ownership-test "${_model_formats_root}/tests/model_formats/test_safetensors_file_ownership.c")
    foreach(_target umicom-safetensors-file-test umicom-safetensors-file-ownership-test)
        target_link_libraries(${_target} PRIVATE Umicom::model_formats)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_target} FILES
                "${_model_formats_root}/src/model_formats/safetensors_file.c"
                "${_model_formats_root}/src/model_formats/safetensors.c"
                "${_model_formats_root}/src/model_formats/header_internal.h"
                "${_model_formats_root}/src/platform/input_stream.c"
                "${_model_formats_root}/tests/model_formats/safetensors_fixture.h")
        endif()
    endforeach()
    foreach(_case IN ITEMS valid
            expected
            borrowed-expected
            wrong-digest
            expected-smaller
            expected-larger
            invalid-expected
            unicode
            missing
            directory
            relative
            null-path
            null-output
            cancel
            truncated
            oversized-header
            invalid-layout
            payload-change
            link)
        set(_test "framework.model_formats.safetensors_file.${_case}")
        add_test(NAME "${_test}" COMMAND umicom-safetensors-file-test "${_case}")
        set_tests_properties("${_test}" PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
            LABELS "framework;model-formats;file;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${_test}" TARGETS umicom-safetensors-file-test)
        endif()
    endforeach()
    foreach(_case IN ITEMS one-byte
            split-length
            split-header
            whole
            zero-chunk
            allocation
            partial-read
            late-close
            changed-file
            late-cancel
            truncated-header)
        set(_test "framework.model_formats.safetensors_file_ownership.${_case}")
        add_test(NAME "${_test}" COMMAND umicom-safetensors-file-ownership-test "${_case}")
        set_tests_properties("${_test}" PROPERTIES TIMEOUT 30
            LABELS "framework;model-formats;file;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${_test}" TARGETS umicom-safetensors-file-ownership-test)
        endif()
    endforeach()
endif()
