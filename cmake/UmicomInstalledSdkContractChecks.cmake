# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Only the owner-run external consumer qualifies real installed archives.
# These smaller fixtures exercise configuration refusal without a compiler.
include_guard(GLOBAL)
if(NOT BUILD_TESTING)
    return()
endif()
set(_sdk_cases valid spaced-prefix source-target missing-identity changed-header extra-header
    outside-library missing-library outside-headers wrong-prefix missing-component)
set(_sdk_source-target "rejects source-built target")
set(_sdk_missing-identity "lacks its full public-header identity")
set(_sdk_changed-header "Installed headers differ")
set(_sdk_extra-header "Installed headers differ")
set(_sdk_outside-library "outside the selected installed SDK")
set(_sdk_missing-library "missing or not absolute")
set(_sdk_outside-headers "outside the selected installed SDK")
set(_sdk_wrong-prefix "not from the requested SDK prefix")
set(_sdk_missing-component "component target is unavailable")
list(APPEND _sdk_cases transitive-outside-library transitive-outside-headers
    transitive-missing transitive-source transitive-cycle transitive-link-only external-target)
set(_sdk_transitive-outside-library "outside the selected installed SDK")
set(_sdk_transitive-outside-headers "outside the selected installed SDK")
set(_sdk_transitive-missing "component target is unavailable")
set(_sdk_transitive-source "rejects source-built target")
list(APPEND _sdk_cases inherited-headers aggregate-headers transitive-inherited-headers missing-base-headers)
set(_sdk_missing-base-headers "base component has no public header directory")
foreach(_case IN LISTS _sdk_cases)
    add_test(NAME "framework.sdk.installed_contract.${_case}"
        COMMAND "${CMAKE_COMMAND}" "-DTEST_CASE=${_case}"
            "-DGENERATOR=${CMAKE_GENERATOR}"
            "-DFIXTURE=${CMAKE_CURRENT_LIST_DIR}/../tests/sdk_contract/fixture"
            "-DOUTPUT_ROOT=${CMAKE_CURRENT_BINARY_DIR}/sdk-contract/${_case}"
            "-DCONTRACT_HELPER=${CMAKE_CURRENT_LIST_DIR}/UmicomRequireInstalledSdk.cmake"
            "-DEXPECTED_ERROR=${_sdk_${_case}}"
            -P "${CMAKE_CURRENT_LIST_DIR}/../tests/sdk_contract/check.cmake")
    set_tests_properties("framework.sdk.installed_contract.${_case}" PROPERTIES
        TIMEOUT 60 LABELS "framework;sdk;configuration;regression")
endforeach()

# The repeated negative fixture deliberately runs two bounded configure processes.
# Keep their combined budget separate from the original single-run cases.
if(TEST framework.sdk.installed_contract.extra-header)
    set_tests_properties(framework.sdk.installed_contract.extra-header PROPERTIES TIMEOUT 120)
endif()
