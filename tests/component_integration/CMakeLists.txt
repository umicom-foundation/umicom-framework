# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Use include(), not add_subdirectory(): this registration may itself be late.
include_guard(GLOBAL)
get_filename_component(_umi_composition_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
foreach(_case IN ITEMS deferred immediate mixed repeated missing partial tests_off reconfigure regenerate installed)
    add_test(NAME framework.component_integration.${_case}
        COMMAND "${CMAKE_COMMAND}"
        "-DCASE=${_case}"
        "-DFRAMEWORK=${_umi_composition_root}"
        "-DPROBE_ROOT=${CMAKE_CURRENT_BINARY_DIR}/component-integration-checks"
        "-DHOST_GENERATOR=${CMAKE_GENERATOR}"
        "-DHOST_COMPILER=${CMAKE_C_COMPILER}"
        "-DHOST_TOOLCHAIN=${CMAKE_TOOLCHAIN_FILE}"
        "-DHOST_PLATFORM=${CMAKE_GENERATOR_PLATFORM}"
        "-DHOST_TOOLSET=${CMAKE_GENERATOR_TOOLSET}"
        "-DHOST_CROSSCOMPILING=${CMAKE_CROSSCOMPILING}"
        -P "${CMAKE_CURRENT_LIST_DIR}/run_case.cmake")
    set_tests_properties(framework.component_integration.${_case} PROPERTIES
        TIMEOUT 240 PROCESSORS 2 LABELS "component-integration;configure;native")
endforeach()
unset(_umi_composition_root)
