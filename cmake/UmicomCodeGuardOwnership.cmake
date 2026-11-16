#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCodeGuardOwnership.cmake
#
# PURPOSE:
# Register deterministic CodeGuard ownership checks without replacing real rule tests.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
if(BUILD_TESTING)
    # The fixture includes the production ownership sources with private
    # allocator/scanner boundaries. Do not link a second CodeGuard copy here.
    add_executable(umicom-codeguard-ownership-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/codeguard/test_transactional_results.c")
    # Service now exposes asynchronous jobs. Compile that dependency once and
    # use real platform tasks; keep the instrumented service/result sources
    # and scanner boundary in the fixture rather than linking CodeGuard twice.
    target_sources(umicom-codeguard-ownership-tests PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../src/codeguard/job.c")
    target_link_libraries(umicom-codeguard-ownership-tests PRIVATE Umicom::platform)
    target_include_directories(umicom-codeguard-ownership-tests PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../include")
    target_compile_features(umicom-codeguard-ownership-tests PRIVATE c_std_23)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(umicom-codeguard-ownership-tests PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    foreach(_case IN ITEMS arguments root-boundary root-copy profile default-profile
        create-failure-1 create-failure-2 create-failure-3 first-failure replace empty
        partial-failure cancelled scan-memory scan-capacity candidate-object candidate-items
        result-capacity result-alias-growth result-alias-no-growth result-growth-failure
        result-growth-boundary result-swap result-swap-empty result-swap-invalid
        token-preflight token-invalid token-late token-error token-null)
        add_test(NAME "framework.codeguard_ownership.${_case}"
            COMMAND umicom-codeguard-ownership-tests "${_case}")
        set_tests_properties("framework.codeguard_ownership.${_case}" PROPERTIES
            TIMEOUT 15 LABELS "codeguard;ownership;fault-injection")
    endforeach()
endif()
