#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomApplicationExperienceIntegration.cmake
#
# PURPOSE:
#   Attach the Framework-owned application experience catalogue to the existing
#   umicom_application target without creating a second build catalogue.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------

if(NOT TARGET umicom_application)
    message(FATAL_ERROR
        "UmicomApplicationExperienceIntegration requires umicom_application")
endif()

get_target_property(
    _umicom_experience_integrated
    umicom_application
    UMICOM_APPLICATION_EXPERIENCE_INTEGRATED)

if(NOT _umicom_experience_integrated)
    # Application runtime metadata now projects into the existing canonical UI models.
    target_link_libraries(umicom_application PUBLIC Umicom::ui)

    target_sources(umicom_application PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experience.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experience_registry.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experience_status.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experience_plan.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experience_catalogue.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/thin_client.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/studio.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/trader.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/ai_creator.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/bank.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/tms.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/llm.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/exchange.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/music.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/media.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/accountant.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/rag.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/desktop.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/os.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/games.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/kitchen.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/cad.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/author.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/web_studio.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/mobile_studio.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/database_studio.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/integration_studio.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/operations.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/security_centre.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/marketplace.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/experiences/education.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime/session.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime/panel_state.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime/layout_session.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime/activation_plan.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime/readiness.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime/feature_gate.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime/capability_status.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime/context_binding.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime/launch_profile.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime/session_snapshot.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime/runtime_health.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime/lifecycle_evidence.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime/panel_host.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime/command_surface.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime/experience_projection.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime/contract_resolver.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime/runtime_catalogue.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime/workspace_catalogue.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime/operation.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime/module_status.c"
        "${CMAKE_CURRENT_LIST_DIR}/../src/application/runtime/workspace_runtime.c"
    )

    if(TARGET umicom_application_ui)
        target_sources(umicom_application_ui PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../src/application_ui/summary_view.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/application_ui/readiness_view.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/application_ui/panel_catalogue_view.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/application_ui/layout_selector_view.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/application_ui/feature_backlog_view.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/application_ui/capability_view.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/application_ui/health_view.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/application_ui/ownership_view.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/application_ui/session_view.c"
        )
    endif()

    set_property(
        TARGET umicom_application
        PROPERTY UMICOM_APPLICATION_EXPERIENCE_INTEGRATED TRUE)

    if(BUILD_TESTING AND NOT TARGET umicom-application-experience-test)
        add_executable(umicom-application-experience-test
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience/test_main.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience/test_catalogue.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience/test_registry.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience/test_status.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience/test_priority.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience/test_profiles.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_experience/test_portfolio_alignment.c"
        )
        target_link_libraries(umicom-application-experience-test PRIVATE
            Umicom::application)
        umicom_apply_warnings(umicom-application-experience-test)
        umicom_apply_sanitizers(umicom-application-experience-test)
        add_test(
            NAME application.experience.catalogue
            COMMAND umicom-application-experience-test)
    endif()

    if(BUILD_TESTING AND NOT TARGET umicom-application-runtime-test)
        add_executable(umicom-application-runtime-test
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_main.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_session.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_panel_state.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_layout_session.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_activation_plan.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_readiness.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_feature_gate.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_capability_status.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_context_binding.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_launch_profile.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_session_snapshot.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_runtime_health.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_lifecycle_evidence.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_panel_host.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_command_surface.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_experience_projection.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_contract_resolver.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_runtime_catalogue.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_workspace_catalogue.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_operation.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_module_status.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_workspace_runtime.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_ui_summary.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_ui_readiness.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_ui_catalogues.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_ui_health_ownership.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_ui_session.c"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_workbench_binding.c"
        )
        if(TARGET Umicom::application_ui)
            target_link_libraries(umicom-application-runtime-test PRIVATE
                Umicom::application_ui)
        else()
            target_link_libraries(umicom-application-runtime-test PRIVATE
                Umicom::application)
        endif()
        umicom_apply_warnings(umicom-application-runtime-test)
        umicom_apply_sanitizers(umicom-application-runtime-test)
        add_test(
            NAME application.experience.runtime
            COMMAND umicom-application-runtime-test)
    endif()

    if(BUILD_TESTING AND NOT TARGET umicom-application-thin-client-test)
        add_executable(umicom-application-thin-client-test
            "${CMAKE_CURRENT_LIST_DIR}/../tests/application_runtime/test_thin_client.c")
        target_link_libraries(umicom-application-thin-client-test PRIVATE
            Umicom::application)
        umicom_apply_warnings(umicom-application-thin-client-test)
        umicom_apply_sanitizers(umicom-application-thin-client-test)
        add_test(
            NAME application.experience.thin_client
            COMMAND umicom-application-thin-client-test)
    endif()

endif()

# Productisation evidence consumes the canonical application experience and
# component catalogues; it does not create parallel product definitions.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationProductisationPlatform.cmake")

# Finance applications reuse one projection layer over the existing finance
# and UI targets; no banking or treasury state is reimplemented here.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomFinanceUiPlatform.cmake")

# Framework completion prerequisites remain Framework-owned and are built once.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomFrameworkCompletionPlatform.cmake")

# MCP and hybrid retrieval extend the existing AI target and policy/tool path.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiMcpPlatform.cmake")

# AI reasoning now feeds bounded proposals into the existing governed Helix path.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiHelixBridge.cmake")
