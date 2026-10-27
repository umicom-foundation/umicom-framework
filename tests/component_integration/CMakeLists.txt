#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/component_integration/CMakeLists.txt
# PURPOSE: Register bounded composition scenarios with complete outer deadlines.
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
# Use include(), not add_subdirectory(): this registration may itself be late.
include_guard(GLOBAL)
get_filename_component(_umi_composition_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
foreach(_case IN ITEMS deferred immediate mixed repeated missing partial tests_off reconfigure regenerate installed)
    add_test(NAME framework.component_integration.${_case}
        COMMAND "${CMAKE_COMMAND}"
        "-DCASE=${_case}"
        "-DFRAMEWORK=${_umi_composition_root}"
        # Long nested fixture paths can exhaust the Windows compiler dependency-file limit.
        # The previous path is retained as a comment for source review.
        # "-DPROBE_ROOT=${CMAKE_CURRENT_BINARY_DIR}/component-integration-checks"
        "-DPROBE_ROOT=${CMAKE_BINARY_DIR}/ci"
        "-DHOST_GENERATOR=${CMAKE_GENERATOR}"
        "-DHOST_COMPILER=${CMAKE_C_COMPILER}"
        "-DHOST_TOOLCHAIN=${CMAKE_TOOLCHAIN_FILE}"
        "-DHOST_PLATFORM=${CMAKE_GENERATOR_PLATFORM}"
        "-DHOST_TOOLSET=${CMAKE_GENERATOR_TOOLSET}"
        "-DHOST_CROSSCOMPILING=${CMAKE_CROSSCOMPILING}"
        -P "${CMAKE_CURRENT_LIST_DIR}/run_case.cmake")
    # Five inventory checks, four rebuilds and the full nested suite share this
    # outer deadline. The old single-stage budget killed passing nested tests.
    # Each Execute operation retains its separate 180-second timeout.
    if(FALSE)
    set_tests_properties(framework.component_integration.${_case} PROPERTIES
        TIMEOUT 240 PROCESSORS 2 LABELS "component-integration;configure;native")
    endif()
    # All workflows need a budget for each bounded stage. Retain the earlier
    # exception-based deadlines for review.
    if(FALSE)
    set(_case_timeout 240)
    # The original exception handled repeated explicit reconfiguration only.
    # Keep it for review; automatic regeneration also adds a second build and
    # inventory before running the complete nested suite. Each subprocess still
    # has its own 180-second deadline, so a stuck step is not given 900 seconds.
    if(FALSE)
    if(_case STREQUAL "reconfigure")
        set(_case_timeout 900)
    endif()
    endif()
    if(_case STREQUAL "reconfigure" OR _case STREQUAL "regenerate")
        set(_case_timeout 900)
    endif()
    endif()
    # Ordinary cases configure, build, inspect inventory and run nested tests.
    # Extra operations each keep their 180-second subprocess deadline. Sum
    # those bounds and add cleanup slack instead of terminating passing stages.
    set(_stage_count 4)
    if(_case STREQUAL "missing" OR _case STREQUAL "tests_off")
        set(_stage_count 3)
    elseif(_case STREQUAL "reconfigure")
        math(EXPR _stage_count "4 + 4 * 3")
    elseif(_case STREQUAL "regenerate")
        math(EXPR _stage_count "4 + 3")
    elseif(_case STREQUAL "installed")
        math(EXPR _stage_count "4 + 4")
    endif()
    math(EXPR _case_timeout "${_stage_count} * 180 + 30")
    set_tests_properties(framework.component_integration.${_case} PROPERTIES
        TIMEOUT ${_case_timeout} PROCESSORS 2 LABELS "component-integration;configure;native")
endforeach()
unset(_stage_count)
unset(_case_timeout)
unset(_umi_composition_root)
