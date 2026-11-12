#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomGuideBrandingChecks.cmake
#
# PURPOSE:
#   Register unified CLI admission and maintained learning-catalogue checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

if(BUILD_TESTING AND TARGET umicom)
    add_executable(umicom-cli-branding-admission-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/test_cli_branding_admission.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_branding.c")
    target_include_directories(umicom-cli-branding-admission-test PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src")
    target_link_libraries(umicom-cli-branding-admission-test PRIVATE Umicom::Framework Umicom::source_contracts)
    umicom_apply_warnings(umicom-cli-branding-admission-test)
    umicom_apply_sanitizers(umicom-cli-branding-admission-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-cli-branding-admission-test)
    endif()
    foreach(_case IN ITEMS help nested-help empty negative too-many null-vector null-item empty-item missing-value missing-root missing-assets duplicate-assets unknown-option option-value unknown-command)
        add_test(NAME "framework.cli.branding.${_case}" COMMAND umicom-cli-branding-admission-test "${_case}")
        set_tests_properties("framework.cli.branding.${_case}" PROPERTIES
            TIMEOUT 10 LABELS "framework;cli;branding;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.cli.branding.${_case}" TARGETS umicom-cli-branding-admission-test)
        endif()
    endforeach()
    add_test(NAME framework.cli.branding.learning
        COMMAND umicom branding audit --assets "${CMAKE_CURRENT_LIST_DIR}/../resources/brand"
            --root "${CMAKE_CURRENT_LIST_DIR}/../docs/learning")
    set_tests_properties(framework.cli.branding.learning PROPERTIES
        TIMEOUT 60 LABELS "framework;cli;branding;learning")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.cli.branding.learning TARGETS umicom ALWAYS)
    endif()
endif()
