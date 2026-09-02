#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomApplicationSuiteLayoutPlatform.cmake
#
# PURPOSE:
#   Attach canonical suite layout projection, render planning, selection and
#   validation to existing Framework application/UI targets.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET umicom_application)
    message(FATAL_ERROR "Application Suite Layout requires canonical umicom_application")
endif()

set(UMICOM_APPLICATION_SUITE_LAYOUT_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

target_sources(umicom_application PRIVATE
    "${UMICOM_APPLICATION_SUITE_LAYOUT_ROOT}/src/application/suite_layout/geometry.c"
    "${UMICOM_APPLICATION_SUITE_LAYOUT_ROOT}/src/application/suite_layout/layout_summary.c"
    "${UMICOM_APPLICATION_SUITE_LAYOUT_ROOT}/src/application/suite_layout/descriptor.c"
    "${UMICOM_APPLICATION_SUITE_LAYOUT_ROOT}/src/application/suite_layout/catalogue.c"
    "${UMICOM_APPLICATION_SUITE_LAYOUT_ROOT}/src/application/suite_layout/customisation.c"
    "${UMICOM_APPLICATION_SUITE_LAYOUT_ROOT}/src/application/suite_layout/projection.c"
    "${UMICOM_APPLICATION_SUITE_LAYOUT_ROOT}/src/application/suite_layout/profile_projection.c"
    "${UMICOM_APPLICATION_SUITE_LAYOUT_ROOT}/src/application/suite_layout/workbench_bridge.c"
    "${UMICOM_APPLICATION_SUITE_LAYOUT_ROOT}/src/application/suite_layout/render_plan.c"
    "${UMICOM_APPLICATION_SUITE_LAYOUT_ROOT}/src/application/suite_layout/selector_model.c"
    "${UMICOM_APPLICATION_SUITE_LAYOUT_ROOT}/src/application/suite_layout/runtime.c"
)

# Keep the source-only documentation check available even when compiled tests
# are disabled, allowing editors and review jobs to run it quickly.
if(NOT TARGET umicom-suite-layout-documentation-audit)
    add_custom_target(umicom-suite-layout-documentation-audit
        COMMAND "${CMAKE_COMMAND}"
            "-DUMICOM_DOCUMENTATION_HEADER_ROOT=${UMICOM_APPLICATION_SUITE_LAYOUT_ROOT}/include/umicom/application/suite_layout"
            "-DUMICOM_DOCUMENTATION_API_PREFIX=umi_application_suite_"
            -P "${CMAKE_CURRENT_LIST_DIR}/UmicomStructuredApiDocumentationAudit.cmake"
        COMMENT "Checking structured suite-layout API documentation"
        VERBATIM)
endif()

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING)
    # Define the add application suite layout test build helper so parent and application
    # projects apply one consistent rule.
    function(umicom_add_application_suite_layout_test target test_name source)
        # Configure the optional target only when its feature has created it.
        if(TARGET "${target}")
            return()
        endif()
        add_executable("${target}"
            "${UMICOM_APPLICATION_SUITE_LAYOUT_ROOT}/tests/application_suite_layout/${source}")
        target_link_libraries("${target}" PRIVATE Umicom::application Umicom::ui)
        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()
        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties("${test_name}" PROPERTIES
            LABELS "framework;application;layout;suite;studio;trader")
        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("${target}")
        endif()
    endfunction()

    # CTest records the same source-only audit beside functional layout tests,
    # making missing public guidance visible in normal release evidence.
    add_test(
        NAME framework.application_suite.layouts.documentation
        COMMAND "${CMAKE_COMMAND}"
            "-DUMICOM_DOCUMENTATION_HEADER_ROOT=${UMICOM_APPLICATION_SUITE_LAYOUT_ROOT}/include/umicom/application/suite_layout"
            "-DUMICOM_DOCUMENTATION_API_PREFIX=umi_application_suite_"
            -P "${CMAKE_CURRENT_LIST_DIR}/UmicomStructuredApiDocumentationAudit.cmake")
    set_tests_properties(
        framework.application_suite.layouts.documentation
        PROPERTIES LABELS "framework;application;layout;suite;documentation")

    umicom_add_application_suite_layout_test(
        umicom-application-suite-layout-test-all-defaults
        framework.application_suite.layouts.all.defaults
        test_all_defaults.c)
    umicom_add_application_suite_layout_test(
        umicom-application-suite-layout-test-catalogue
        framework.application_suite.layouts.catalogue
        test_catalogue.c)
    umicom_add_application_suite_layout_test(
        umicom-application-suite-layout-summary-test
        framework.application_suite.layouts.summary
        test_layout_summary.c)
    umicom_add_application_suite_layout_test(
        umicom-application-suite-layout-test-catalogue-named-products
        framework.application_suite.layouts.catalogue.named.products
        test_catalogue_named_products.c)
    umicom_add_application_suite_layout_test(
        umicom-application-suite-layout-test-groups
        framework.application_suite.layouts.groups
        test_groups.c)
    umicom_add_application_suite_layout_test(
        umicom-application-suite-layout-test-invalid-layout
        framework.application_suite.layouts.invalid.layout
        test_invalid_layout.c)
    umicom_add_application_suite_layout_test(
        umicom-application-suite-layout-test-missing-application
        framework.application_suite.layouts.missing.application
        test_missing_application.c)
    umicom_add_application_suite_layout_test(
        umicom-application-suite-layout-test-profile-projection
        framework.application_suite.layouts.profile.projection
        test_profile_projection.c)
    umicom_add_application_suite_layout_test(
        umicom-application-suite-layout-test-regions
        framework.application_suite.layouts.regions
        test_regions.c)
    umicom_add_application_suite_layout_test(
        umicom-application-suite-layout-test-studio
        framework.application_suite.layouts.studio
        test_studio.c)
    umicom_add_application_suite_layout_test(
        umicom-application-suite-layout-test-trader
        framework.application_suite.layouts.trader
        test_trader.c)
    umicom_add_application_suite_layout_test(
        umicom-application-suite-layout-render-plan-test
        framework.application_suite.layouts.render.plan
        test_render_plan.c)
    umicom_add_application_suite_layout_test(
        umicom-application-suite-layout-render-all-test
        framework.application_suite.layouts.render.all
        test_render_all.c)
    umicom_add_application_suite_layout_test(
        umicom-application-suite-layout-selector-test
        framework.application_suite.layouts.selector
        test_selector_model.c)
    umicom_add_application_suite_layout_test(
        umicom-application-suite-layout-selector-invalid-test
        framework.application_suite.layouts.selector.invalid
        test_selector_invalid.c)
    umicom_add_application_suite_layout_test(
        umicom-application-suite-layout-resolved-profile-test
        framework.application_suite.layouts.profile.resolved
        test_resolved_profile.c)
    umicom_add_application_suite_layout_test(
        umicom-application-suite-layout-abi-compatibility-test
        framework.application_suite.layouts.abi.compatibility
        test_abi_compatibility.c)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationSuiteGtk4Platform.cmake")

# Guarded professional trading interactions and deterministic simulation extend
# the same canonical suite-layout/GTK4 path used by thin Trader and Studio.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomTradingWorkstationInteractionPlatform.cmake")
