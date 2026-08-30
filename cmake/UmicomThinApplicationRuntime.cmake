#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomThinApplicationRuntime.cmake
#
# PURPOSE:
#   Attach the shared thin-application runtime, readiness and workspace-command
#   composition files and their validation targets without duplicating CMake
#   target-registration logic across application repositories.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(CMakeParseArguments)

function(umicom_extend_thin_application_runtime)
    set(options)
    set(one_value_arguments NAME TARGET)
    set(multi_value_arguments)
    cmake_parse_arguments(
        UMI_THIN_RUNTIME
        "${options}"
        "${one_value_arguments}"
        "${multi_value_arguments}"
        ${ARGN})

    foreach(required_argument NAME TARGET)
        if(NOT UMI_THIN_RUNTIME_${required_argument})
            message(FATAL_ERROR
                "Thin application runtime requires ${required_argument}")
        endif()
    endforeach()
    if(NOT TARGET "${UMI_THIN_RUNTIME_TARGET}")
        message(FATAL_ERROR
            "Thin application runtime target '${UMI_THIN_RUNTIME_TARGET}' does not exist")
    endif()

    get_target_property(
        runtime_attached
        "${UMI_THIN_RUNTIME_TARGET}"
        UMICOM_THIN_APPLICATION_RUNTIME_ATTACHED)
    if(runtime_attached)
        message(FATAL_ERROR
            "Thin application runtime is already attached to '${UMI_THIN_RUNTIME_TARGET}'")
    endif()
    set_target_properties(
        "${UMI_THIN_RUNTIME_TARGET}"
        PROPERTIES UMICOM_THIN_APPLICATION_RUNTIME_ATTACHED TRUE)

    target_sources("${UMI_THIN_RUNTIME_TARGET}" PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/runtime.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/readiness.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/workspace_commands.c")

    if(BUILD_TESTING)
        set(runtime_test_names
            runtime
            readiness_runtime
            workspace_commands)
        foreach(runtime_test_name IN LISTS runtime_test_names)
            string(REPLACE "_" "-" runtime_test_suffix
                "${runtime_test_name}")
            set(runtime_test_target
                "${UMI_THIN_RUNTIME_TARGET}-${runtime_test_suffix}-test")
            if(TARGET "${runtime_test_target}")
                message(FATAL_ERROR
                    "Duplicate thin application test target '${runtime_test_target}'")
            endif()
            add_executable("${runtime_test_target}"
                "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_${runtime_test_name}.c")
            target_link_libraries("${runtime_test_target}" PRIVATE
                "${UMI_THIN_RUNTIME_TARGET}")
            if(COMMAND umicom_apply_warnings)
                umicom_apply_warnings("${runtime_test_target}")
            endif()
            if(COMMAND umicom_apply_sanitizers)
                umicom_apply_sanitizers("${runtime_test_target}")
            endif()
            add_test(
                NAME "${UMI_THIN_RUNTIME_NAME}.module.${runtime_test_name}"
                COMMAND "${runtime_test_target}")
            set_tests_properties(
                "${UMI_THIN_RUNTIME_NAME}.module.${runtime_test_name}"
                PROPERTIES LABELS
                    "application;thin-runtime;${UMI_THIN_RUNTIME_NAME}")
            if(COMMAND umicom_register_validation_target)
                umicom_register_validation_target("${runtime_test_target}")
            endif()
        endforeach()
    endif()
endfunction()
