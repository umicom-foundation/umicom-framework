#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomUserPresetChecks.cmake
#
# PURPOSE:
#   Register bounded preset formatting, file preservation and CMake interpretation coverage.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
target_sources(umicom_toolchain PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/user_presets.c")
if(NOT BUILD_TESTING)
    return()
endif()
set(_user_presets_cases valid quotes controls unicode macros maximum empty-prefix small-output null-output zero-output null-profile profile-size profile-version profile-count not-compiler missing-compiler missing-ninja missing-pkg slot-kind empty-compiler compiler-limit ninja-limit pkg-limit prefix-limit invalid-utf8)
set(_user_presets_files_cases new preview existing existing-preview racing-file write-failure close-failure create-failure invalid-profile relative-root outside-root same-root missing-parent invalid-preview legacy-replace)
foreach(suite user_presets user_presets_files)
    set(target "umicom-${suite}-test")
    add_executable(${target} "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_lifecycle/test_${suite}.c")
    target_link_libraries(${target} PRIVATE Umicom::toolchain Umicom::protocol)
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(${target})
    umicom_apply_sanitizers(${target})
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${target})
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(${target} FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/user_presets.c"
            "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/toolchain/user_presets.h")
    endif()
    foreach(scenario IN LISTS _${suite}_cases)
        set(test "framework.toolchain.${suite}.${scenario}")
        add_test(NAME ${test} COMMAND ${target} ${scenario})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30 LABELS "framework;toolchain;presets;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS ${target})
        endif()
    endforeach()
endforeach()
foreach(scenario preset-existing preset-invalid preset-race)
    set(test "framework.toolchain.cache_repair_cli.${scenario}")
    add_test(NAME ${test} COMMAND umicom-cache_repair_cli-test ${scenario})
    set_tests_properties(${test} PROPERTIES TIMEOUT 30 LABELS "framework;toolchain;presets;cli;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(${test} TARGETS umicom-cache_repair_cli-test)
    endif()
endforeach()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom-cache_repair_cli-test FILES
        "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/toolchain/user_presets.h")
endif()
add_test(NAME framework.toolchain.user_presets.interpretation
    COMMAND "${CMAKE_COMMAND}" "-DTOOL=$<TARGET_FILE:umicom-user_presets-test>"
        -P "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_lifecycle/check_user_presets.cmake")
set_tests_properties(framework.toolchain.user_presets.interpretation PROPERTIES
    TIMEOUT 60 SKIP_REGULAR_EXPRESSION "SKIP:" LABELS "framework;toolchain;presets;cmake;regression")
if(COMMAND umicom_test_impact_owners)
    umicom_test_impact_owners(framework.toolchain.user_presets.interpretation TARGETS umicom-user_presets-test)
endif()
