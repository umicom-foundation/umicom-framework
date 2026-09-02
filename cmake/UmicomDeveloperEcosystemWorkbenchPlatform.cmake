#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomDeveloperEcosystemWorkbenchPlatform.cmake
#
# PURPOSE:
#   Compose existing plug-in, SDK, distribution, project and designer engines
#   into a package/extension/component developer workbench. Owning engines are
#   extended through projections and review plans; none is duplicated here.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_DEVELOPER_ECOSYSTEM_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET umicom_developer OR NOT TARGET umicom_plugin OR
   NOT TARGET umicom_sdk_runtime OR NOT TARGET umicom_distribution OR
   NOT TARGET umicom_sdk OR NOT TARGET umicom_designer)
    message(FATAL_ERROR
        "Developer Ecosystem Workbench requires developer, plugin, SDK runtime, "
        "distribution, SDK and designer targets")
endif()

target_sources(umicom_developer PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer/ecosystem/types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer/ecosystem/source_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer/ecosystem/package_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer/ecosystem/package_query.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer/ecosystem/install_review.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer/ecosystem/update_review.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer/ecosystem/sdk_inventory.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer/ecosystem/component_wizard.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer/ecosystem/migration_review.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer/ecosystem/provenance_review.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer/ecosystem/command_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/developer/ecosystem/runtime.c"
)

target_link_libraries(umicom_developer PUBLIC
    Umicom::plugin
    Umicom::sdk_runtime
    Umicom::distribution
    Umicom::sdk
    Umicom::designer
)

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING)
    # Define the add developer ecosystem test build helper so parent and application projects
    # apply one consistent rule.
    function(umicom_add_developer_ecosystem_test target test_name source)
        # Configure the optional target only when its feature has created it.
        if(TARGET "${target}")
            return()
        endif()
        add_executable("${target}" "${UMICOM_DEVELOPER_ECOSYSTEM_ROOT}/${source}")
        target_link_libraries("${target}" PRIVATE Umicom::Framework)
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
            LABELS "framework;developer-ecosystem;packages;sdk;extensions")
        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("${target}")
        endif()
    endfunction()

    umicom_add_developer_ecosystem_test(umicom-developer-ecosystem-types-test
        framework.developer_ecosystem.types tests/developer_ecosystem/test_types.c)
    umicom_add_developer_ecosystem_test(umicom-developer-ecosystem-sources-test
        framework.developer_ecosystem.sources tests/developer_ecosystem/test_source_catalogue.c)
    umicom_add_developer_ecosystem_test(umicom-developer-ecosystem-packages-test
        framework.developer_ecosystem.packages tests/developer_ecosystem/test_package_catalogue.c)
    umicom_add_developer_ecosystem_test(umicom-developer-ecosystem-query-test
        framework.developer_ecosystem.query tests/developer_ecosystem/test_package_query.c)
    umicom_add_developer_ecosystem_test(umicom-developer-ecosystem-install-test
        framework.developer_ecosystem.install_review tests/developer_ecosystem/test_install_review.c)
    umicom_add_developer_ecosystem_test(umicom-developer-ecosystem-updates-test
        framework.developer_ecosystem.update_review tests/developer_ecosystem/test_update_review.c)
    umicom_add_developer_ecosystem_test(umicom-developer-ecosystem-sdk-test
        framework.developer_ecosystem.sdk_inventory tests/developer_ecosystem/test_sdk_inventory.c)
    umicom_add_developer_ecosystem_test(umicom-developer-ecosystem-component-test
        framework.developer_ecosystem.component_wizard tests/developer_ecosystem/test_component_wizard.c)
    umicom_add_developer_ecosystem_test(umicom-developer-ecosystem-migration-test
        framework.developer_ecosystem.migration_review tests/developer_ecosystem/test_migration_review.c)
    umicom_add_developer_ecosystem_test(umicom-developer-ecosystem-provenance-test
        framework.developer_ecosystem.provenance_review tests/developer_ecosystem/test_provenance_review.c)
    umicom_add_developer_ecosystem_test(umicom-developer-ecosystem-commands-test
        framework.developer_ecosystem.commands tests/developer_ecosystem/test_command_catalogue.c)
    umicom_add_developer_ecosystem_test(umicom-developer-ecosystem-runtime-test
        framework.developer_ecosystem.runtime tests/developer_ecosystem/test_runtime.c)
endif()
