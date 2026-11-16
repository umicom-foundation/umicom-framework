# Umicom Framework
# File: cmake/UmicomGgufInspection.cmake
#
# PURPOSE:
#   Compose incremental GGUF metadata and complete-file inspection within the existing model format SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
if(NOT TARGET Umicom::model_formats)
    return()
endif()
get_filename_component(_gguf_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
target_sources(umicom_model_formats PRIVATE
    "${_gguf_root}/src/model_formats/gguf.c"
    "${_gguf_root}/src/model_formats/gguf_file.c"
    "${_gguf_root}/src/model_formats/gguf_internal.h")
install(FILES "${_gguf_root}/include/umicom/model_formats/gguf.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/model_formats" COMPONENT Framework)
if(BUILD_TESTING)
    add_executable(umicom-gguf-metadata-test "${_gguf_root}/tests/model_formats/test_gguf.c")
    add_executable(umicom-gguf-file-ownership-test "${_gguf_root}/tests/model_formats/test_gguf_file_ownership.c")
    add_executable(umicom-gguf-file-test "${_gguf_root}/tests/model_formats/test_gguf_file.c")
    foreach(_target umicom-gguf-metadata-test umicom-gguf-file-ownership-test umicom-gguf-file-test)
        target_link_libraries(${_target} PRIVATE Umicom::model_formats)
        set_target_properties(${_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_target} FILES
                "${_gguf_root}/src/model_formats/gguf.c"
                "${_gguf_root}/src/model_formats/gguf_file.c"
                "${_gguf_root}/src/model_formats/gguf_internal.h"
                "${_gguf_root}/include/umicom/model_formats/gguf.h"
                "${_gguf_root}/src/platform/input_stream.c"
                "${_gguf_root}/tests/model_formats/gguf_fixture.h")
        endif()
    endforeach()
    foreach(_case IN ITEMS controlvector
            projector
            adapter
            adapter-key
            declared-model
            unknown-type
            no-architecture
            no-metadata
            vocabulary
            version-two
            version-one
            big-endian
            magic
            duplicate-architecture
            duplicate-type
            duplicate-adapter
            nonstring-role
            empty-role
            long-role
            maximum-role
            nul-role
            nonascii-role
            empty-key
            key-bound
            maximum-key
            nul-key
            unknown-value
            bool-invalid
            scalar-types
            string-array
            numeric-array
            array-bool-invalid
            empty-array
            array-value-bound
            array-type
            nested-arrays
            array-depth-bound
            entry-bound
            string-byte-bound
            raw-unselected-string
            trailing-payload
            valid
            truncation
            null-prefix
            null-output
            cancel
            copied-output)
        set(_name "framework.model_formats.gguf_metadata.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-gguf-metadata-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;model-formats;gguf;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${_name}" TARGETS umicom-gguf-metadata-test)
        endif()
    endforeach()
    foreach(_case IN ITEMS one-byte
            whole
            zero-chunk
            late-close
            changed-file
            late-cancel
            partial-read
            truncated
            expected
            wrong-digest
            expected-smaller
            expected-larger
            invalid-expected
            null-path
            null-output
            pre-cancel
            payload-change
            split-fields
            borrowed-expected)
        set(_name "framework.model_formats.gguf_ownership.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-gguf-file-ownership-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;model-formats;gguf;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${_name}" TARGETS umicom-gguf-file-ownership-test)
        endif()
    endforeach()
    foreach(_case IN ITEMS unicode
            auxiliary
            directory
            relative
            missing
            valid
            expected)
        set(_name "framework.model_formats.gguf_file.${_case}")
        add_test(NAME "${_name}" COMMAND umicom-gguf-file-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30 LABELS "framework;model-formats;gguf;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${_name}" TARGETS umicom-gguf-file-test)
        endif()
    endforeach()
endif()
unset(_gguf_root)
