#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCodeGuardDuplicateEvidence.cmake
#
# PURPOSE:
# Register duplicate collection and streaming fingerprint evidence checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
if(BUILD_TESTING)
    foreach(_kind IN ITEMS duplicate hash)
        add_executable(umicom-codeguard-${_kind}-failure-tests
            "${CMAKE_CURRENT_LIST_DIR}/../tests/codeguard/test_${_kind}_failures.c")
        target_link_libraries(umicom-codeguard-${_kind}-failure-tests PRIVATE Umicom::codeguard)
        target_compile_features(umicom-codeguard-${_kind}-failure-tests PRIVATE c_std_23)
        umicom_apply_warnings(umicom-codeguard-${_kind}-failure-tests)
        umicom_apply_sanitizers(umicom-codeguard-${_kind}-failure-tests)
    endforeach()
    set(_duplicate_cases arguments longpath path-boundary first-allocation hash-failure hash-capacity growth-failure alias-growth growth-boundary full-capacity invalid-count invalid-capacity missing-storage raw normalized different empty same-hash-different-size first-emission second-emission emission-capacity pair-order)
    set(_hash_cases arguments ordinary comments literals escape slash empty open read close read-close boundary binary)
    foreach(_kind IN ITEMS duplicate hash)
        foreach(_case IN LISTS _${_kind}_cases)
            add_test(NAME "framework.codeguard_${_kind}_failure.${_case}"
                COMMAND umicom-codeguard-${_kind}-failure-tests "${_case}")
            set_tests_properties("framework.codeguard_${_kind}_failure.${_case}" PROPERTIES
                TIMEOUT 20 LABELS "codeguard;ownership;fault-injection")
        endforeach()
    endforeach()
endif()
