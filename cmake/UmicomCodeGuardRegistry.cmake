#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCodeGuardRegistry.cmake
#
# PURPOSE:
# Register deterministic rule registry ownership and capacity checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
if(BUILD_TESTING)
    add_executable(umicom-codeguard-registry-ownership-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/codeguard/test_registry_ownership.c")
    target_link_libraries(umicom-codeguard-registry-ownership-tests PRIVATE Umicom::codeguard)
    target_compile_features(umicom-codeguard-registry-ownership-tests PRIVATE c_std_23)
    umicom_apply_warnings(umicom-codeguard-registry-ownership-tests)
    umicom_apply_sanitizers(umicom-codeguard-registry-ownership-tests)
    foreach(_case IN ITEMS arguments empty first-allocation excess-count null-first
        null-later ordinary descriptor-copy borrowed-strings optional-text empty-needle
        builtins alias-growth alias-without-growth growth-failure alias-failure
        null-batch-rollback count-overflow invalid-count invalid-capacity missing-storage
        growth-boundary full-capacity empty-populated growth-success)
        add_test(NAME "framework.codeguard_registry_ownership.${_case}"
            COMMAND umicom-codeguard-registry-ownership-tests "${_case}")
        set_tests_properties("framework.codeguard_registry_ownership.${_case}" PROPERTIES
            TIMEOUT 20 LABELS "codeguard;ownership;capacity;fault-injection")
    endforeach()
endif()
