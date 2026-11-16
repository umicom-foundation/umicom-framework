# Umicom Framework
# File: cmake/UmicomOllamaPullTests.cmake
#
# PURPOSE:
#   Register the same bounded transcript command checks for native products and installed SDK clients.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
function(umicom_add_pull_inspection_command_tests target prefix)
    cmake_parse_arguments(ARG "STANDALONE_MODELS" "" "" ${ARGN})
    if(ARG_UNPARSED_ARGUMENTS OR NOT TARGET "${target}" OR prefix STREQUAL "")
        message(FATAL_ERROR "Pull inspection tests require an existing host and a nonempty prefix")
    endif()
    set(driver_root "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/ai")
    if(NOT EXISTS "${driver_root}/check_pull_command.cmake")
        set(driver_root "${UMICOM_FRAMEWORK_PULL_TEST_ROOT}")
    endif()
    if(NOT EXISTS "${driver_root}/check_pull_command.cmake")
        message(FATAL_ERROR "Selected Framework lacks the model-progress qualification driver")
    endif()
    foreach(mode success error incomplete malformed duplicate empty relative missing directory unicode help usage extra private-messages crlf no-final-lf after-terminal line-limit file-limit)
        add_test(NAME "${prefix}.${mode}" COMMAND "${CMAKE_COMMAND}"
            "-DHOST_EXE=$<TARGET_FILE:${target}>"
            "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/tests/${prefix}/${mode}"
            "-DTEST_CASE=${mode}" "-DSTANDALONE_MODELS=${ARG_STANDALONE_MODELS}"
            -P "${driver_root}/check_pull_command.cmake")
        set_tests_properties("${prefix}.${mode}" PROPERTIES TIMEOUT 80 LABELS "ai;ollama;offline;console")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("${prefix}.${mode}" TARGETS "${target}")
        endif()
    endforeach()
endfunction()
