#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomReviewedStreamGtk.cmake
#
# PURPOSE:
# Register existing-control streaming and retained library lifetime qualification.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(TARGET umicom_ai_workspace_gtk4)
    # Checked GTK ownership uses the same Platform thread identity contract.
    target_link_libraries(umicom_ai_workspace_gtk4 PRIVATE Umicom::platform)
endif()
if(BUILD_TESTING AND UMICOM_AI_WORKSPACE_HTTP_BUILT AND TARGET Umicom::ai_workspace_gtk4)
    umicom_local_delta_test_target(umicom-reviewed-stream-panel-gtk-test test_reviewed_panel_gtk4.c)
    target_sources(umicom-reviewed-stream-panel-gtk-test PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_workspace/local_http_peer.c")
    target_link_libraries(umicom-reviewed-stream-panel-gtk-test PRIVATE
        Umicom::ai_workspace_gtk4 Umicom::web Umicom::platform)
    if(WIN32)
        target_link_libraries(umicom-reviewed-stream-panel-gtk-test PRIVATE ws2_32)
    endif()
    foreach(_case IN ITEMS arguments wrong-owner success cancel close-active join-failure release-failure
            poll-failure copy-failure try-failure reentrant-close publication-failure start-refusal range-append)
        set(_name "framework.ai_workspace.reviewed_panel.gtk4.${_case}")
        umicom_local_delta_case("${_name}" umicom-reviewed-stream-panel-gtk-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30
            LABELS "framework;ai-workspace;stream;gtk4;ownership")
    endforeach()
    umicom_local_delta_test_target(umicom-reviewed-stream-surface-gtk-test test_reviewed_surface_gtk4.c)
    target_link_libraries(umicom-reviewed-stream-surface-gtk-test PRIVATE
        Umicom::ai_workspace_gtk4 Umicom::platform)
    foreach(_case IN ITEMS attach wrong-owner switch-retain release-retain workspace-release runtime-release candidate-retain)
        set(_name "framework.ai_workspace.reviewed_surface.gtk4.${_case}")
        umicom_local_delta_case("${_name}" umicom-reviewed-stream-surface-gtk-test "${_case}")
        set_tests_properties("${_name}" PROPERTIES TIMEOUT 30
            LABELS "framework;ai-workspace;stream;gtk4;ownership;storage")
    endforeach()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-reviewed-stream-panel-gtk-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/ai_workspace_panel_gtk4.c"
            "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/ai_workspace_reviewed_stream.inc"
            "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/ai_workspace_import.inc"
            "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/ai_workspace_evidence_export.inc"
            "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/ai_workspace_source_library.inc")
        umicom_target_qualification_inputs(umicom-reviewed-stream-surface-gtk-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/ai_workspace_surface_gtk4.c")
    endif()
endif()
