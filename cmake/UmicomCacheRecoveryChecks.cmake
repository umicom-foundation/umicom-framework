#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCacheRecoveryChecks.cmake
#
# PURPOSE:
#   Compose shared cache inspection, recovery regression coverage and the installed learning example.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

include_guard(GLOBAL)
target_sources(umicom_toolchain PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/cmake_cache.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/cache_repair.c")
install(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../examples/cache_inspection/main.c"
    "${CMAKE_CURRENT_LIST_DIR}/../examples/cache_inspection/CMakeLists.txt"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/examples/cache_inspection COMPONENT Learning)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/build-cache-recovery.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/docs/learning COMPONENT Learning)
if(NOT BUILD_TESTING)
    return()
endif()
set(_cmake_cache_cases exact prefix comments comment-only quoted-key crlf bom no-final-newline literal optional-empty unknown prefix-key duplicate duplicate-empty wrong-type missing-type missing-equals tab-value late-malformed leading-space empty embedded-nul invalid-utf8 field-limit byte-limit null-input null-output)
set(_cache_repair_cases healthy missing stale-preview stale-apply prefix comment-only duplicate read-error read-busy read-limit invalid-flag null-profile null-build relative-build root-build build-long join-limit compiler-long compiler-relative compiler-missing compiler-kind profile-size output-pair-null output-pair-zero small-output no-output collision racing-collision rename-failure close-failure graph-failure)
set(_cache_repair_files_cases healthy missing preview apply collision racing-collision malformed cache-directory)
set(_cache_repair_cli_cases healthy preview first-join second-join repair-failure partial-rename preset-failure)
foreach(suite cmake_cache cache_repair cache_repair_files cache_repair_cli)
    set(target "umicom-${suite}-test")
    add_executable(${target} "${CMAKE_CURRENT_LIST_DIR}/../tests/toolchain_lifecycle/test_${suite}.c")
    target_link_libraries(${target} PRIVATE Umicom::toolchain)
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(${target})
    umicom_apply_sanitizers(${target})
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${target})
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(${target} FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/cmake_cache.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/toolchain/cache_repair.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_build.c")
    endif()
    foreach(scenario IN LISTS _${suite}_cases)
        set(test "framework.toolchain.${suite}.${scenario}")
        add_test(NAME ${test} COMMAND ${target} ${scenario})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30 LABELS "framework;toolchain;cache;regression")
        if(suite STREQUAL "cache_repair_files")
            set_tests_properties(${test} PROPERTIES SKIP_RETURN_CODE 77)
        endif()
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS ${target})
        endif()
    endforeach()
endforeach()
# The output remains essential when a native move completed before close failed.
# Evidence is asserted by the fixture; the earlier output-only policy is
# retained for review because PASS_REGULAR_EXPRESSION can override exit status.
#[[
set_tests_properties(framework.toolchain.cache_repair_cli.partial-rename PROPERTIES
    PASS_REGULAR_EXPRESSION "Recorded cache recovery path: recovery-completed")
]]
set_tests_properties(framework.toolchain.cache_repair_cli.healthy PROPERTIES
    FAIL_REGULAR_EXPRESSION "cache recovery path:")
add_executable(umicom-cache-inspection "${CMAKE_CURRENT_LIST_DIR}/../examples/cache_inspection/main.c")
target_link_libraries(umicom-cache-inspection PRIVATE Umicom::toolchain)
set_target_properties(umicom-cache-inspection PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umicom-cache-inspection)
umicom_apply_sanitizers(umicom-cache-inspection)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-cache-inspection)
endif()
foreach(scenario inspect duplicate prefix)
    set(test "framework.toolchain.cache_inspection.${scenario}")
    add_test(NAME ${test} COMMAND umicom-cache-inspection ${scenario})
    set_tests_properties(${test} PROPERTIES TIMEOUT 30 LABELS "framework;toolchain;cache;learning")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(${test} TARGETS umicom-cache-inspection)
    endif()
endforeach()
