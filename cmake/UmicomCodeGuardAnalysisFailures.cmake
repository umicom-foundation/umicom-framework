#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCodeGuardAnalysisFailures.cmake
#
# PURPOSE:
# Register deterministic lifetime, source measurement and architecture failure checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
if(BUILD_TESTING)
    foreach(_kind IN ITEMS lifetime analysis)
        # Each fixture includes its production analysis sources once, with
        # narrow failure hooks. Other services come from the shared library.
        add_executable(umicom-codeguard-${_kind}-failure-tests
            "${CMAKE_CURRENT_LIST_DIR}/../tests/codeguard/test_${_kind}_failures.c")
        target_link_libraries(umicom-codeguard-${_kind}-failure-tests PRIVATE Umicom::codeguard)
        target_compile_features(umicom-codeguard-${_kind}-failure-tests PRIVATE c_std_23)
        umicom_apply_warnings(umicom-codeguard-${_kind}-failure-tests)
        umicom_apply_sanitizers(umicom-codeguard-${_kind}-failure-tests)
    endforeach()
    set(_lifetime_cases arguments invalid-count capacity reuse realloc realloc-failure partial-line assignment-rollback legacy double-free double-free-failure use-after-free use-after-free-failure capacity-status)
    set(_analysis_cases measure-arguments measure-longpath measure-boundary measure-open measure-read measure-close measure-alias measure-empty measure-success architecture-arguments metrics-includes metrics-lines metrics-first-failure metrics-second-failure metrics-capacity studio-success studio-first-failure studio-second-failure measurement-read measurement-close architecture-open architecture-read architecture-close header-first-failure header-second-failure header-success)
    foreach(_kind IN ITEMS lifetime analysis)
        foreach(_case IN LISTS _${_kind}_cases)
            add_test(NAME "framework.codeguard_${_kind}_failure.${_case}"
                COMMAND umicom-codeguard-${_kind}-failure-tests "${_case}")
            set_tests_properties("framework.codeguard_${_kind}_failure.${_case}" PROPERTIES
                TIMEOUT 20 LABELS "codeguard;ownership;fault-injection")
        endforeach()
    endforeach()
endif()
