#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomVideoSequence.cmake
#
# PURPOSE:
#   Compose video editing, rendering and native workspace regression coverage.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
# One creative model supplies native hosts. No encoder download occurs here.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomProcessChannel.cmake")
target_sources(umicom_creative_workspace PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/video_sequence.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/video_relink.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/video_sequence_history.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/video_sequence_document.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/video_render_plan.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/video_soundtrack.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/video_captions.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/video_render.c")
target_link_libraries(umicom_creative_workspace PRIVATE Umicom::process_channel Umicom::developer Umicom::ai)
if(TARGET umicom_creative_workspace_gtk4)
    target_sources(umicom_creative_workspace_gtk4 PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/video_sequence_gtk4.c"
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/video_sequence_relink_gtk4.c"
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/video_sequence_jobs_gtk4.c"
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/video_sequence_preview_gtk4.c")
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ui/gtk4/video_sequence.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/video-sequences.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Framework)

if(BUILD_TESTING)
    function(umicom_video_cases target source)
        add_executable(${target} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/video_sequence/${source}")
        target_link_libraries(${target} PRIVATE Umicom::creative_workspace)
        umicom_creative_configure_target(${target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
        foreach(case IN LISTS ARGN)
            add_test(NAME framework.video_sequence.${target}.${case} COMMAND ${target} ${case})
            set_tests_properties(framework.video_sequence.${target}.${case}
                PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;video-sequence;regression")
        endforeach()
    endfunction()
    umicom_video_cases(umicom-video-edits-test test_edits.c
        split reorder trim remove bounds alias history)
    umicom_video_cases(umicom-video-relink-test test_relink.c
        single batch swap-alias cancelled roundtrip history noop-redo failed-history branch-history
        null count stale missing duplicate relative unterminated invalid-id invalid-sequence)
    umicom_video_cases(umicom-video-document-test test_document.c
        roundtrip strict unchanged cancelled)
    umicom_video_cases(umicom-video-plan-test test_plan.c
        arguments captured limits)
    umicom_video_cases(umicom-video-soundtrack-test test_soundtrack.c arguments invalid)
    umicom_video_cases(umicom-video-captions-test test_captions.c arguments invalid)
    if(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
        # The fixture is a native process, not a script interpreted by a shell.
        add_executable(umicom-video-tool-fixture
            "${CMAKE_CURRENT_LIST_DIR}/../tests/video_sequence/tool_fixture.c")
        target_link_libraries(umicom-video-tool-fixture PRIVATE Umicom::platform)
        umicom_creative_configure_target(umicom-video-tool-fixture)
        add_executable(umicom-video-render-test "${CMAKE_CURRENT_LIST_DIR}/../tests/video_sequence/test_render.c")
        target_link_libraries(umicom-video-render-test PRIVATE Umicom::creative_workspace)
        umicom_creative_configure_target(umicom-video-render-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-video-tool-fixture)
            umicom_register_validation_target(umicom-video-render-test)
        endif()
        foreach(case stream diagnostics failed empty cancelled timeout existing missing-tool late-cancel closed-output)
            add_test(NAME framework.video_sequence.render.${case} COMMAND umicom-video-render-test
                ${case} $<TARGET_FILE:umicom-video-tool-fixture>)
            set_tests_properties(framework.video_sequence.render.${case}
                PROPERTIES TIMEOUT 30 LABELS "framework;video-sequence;process;regression")
        endforeach()
    endif()
    if(TARGET umicom_creative_workspace_gtk4)
        umicom_video_cases(umicom-video-workspace-gtk-test test_workspace_gtk4.c
            geometry selection edits retirement composition music-composition
            library-search library-selection library-empty library-reentry timeline-zoom
            layout-resize layout-visibility layout-recall layout-reset layout-retirement
            layout-reentry layout-retire-notify
            workspace-navigation media-brand media-mode-retention)
        target_link_libraries(umicom-video-workspace-gtk-test PRIVATE Umicom::creative_workspace_gtk4)
        foreach(case geometry selection edits retirement composition music-composition
            library-search library-selection library-empty library-reentry timeline-zoom
            layout-resize layout-visibility layout-recall layout-reset layout-retirement
            layout-reentry layout-retire-notify
            workspace-navigation media-brand media-mode-retention)
            set_tests_properties(framework.video_sequence.umicom-video-workspace-gtk-test.${case}
                PROPERTIES ENVIRONMENT "G_DEBUG=fatal-warnings" LABELS "framework;video-sequence;gtk4;workflow;regression")
        endforeach()
        umicom_video_cases(umicom-video-relink-gtk-test test_relink_gtk4.c
            selected shared reentry draft invalid busy retired selection-move selection-remove empty)
        target_link_libraries(umicom-video-relink-gtk-test PRIVATE Umicom::creative_workspace_gtk4)
        umicom_video_cases(umicom-video-gtk-test test_gtk4.c
            edits save-open approval failed-open captured retained-control close-busy composition completion-reentry saved-undo
            cancel-open retire-dispatch retire-complete retire-refresh)
        target_link_libraries(umicom-video-gtk-test PRIVATE Umicom::creative_workspace_gtk4)
        if(TARGET umicom-video-tool-fixture)
            add_dependencies(umicom-video-gtk-test umicom-video-tool-fixture)
            foreach(case captions-export soundtrack-export render-export)
                add_test(NAME framework.video_sequence.gtk.${case} COMMAND umicom-video-gtk-test
                    ${case} $<TARGET_FILE:umicom-video-tool-fixture>)
                set_tests_properties(framework.video_sequence.gtk.${case} PROPERTIES
                    TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;video-sequence;gtk4;ownership;regression")
            endforeach()
        endif()
    endif()
    set(UMICOM_VIDEO_FFMPEG_EXECUTABLE "" CACHE FILEPATH "Explicit FFmpeg executable for real video render qualification")
    if(UMICOM_VIDEO_FFMPEG_EXECUTABLE AND (WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Linux"))
        add_executable(umicom-video-encoder-test "${CMAKE_CURRENT_LIST_DIR}/../tests/video_sequence/test_encoder.c")
        target_link_libraries(umicom-video-encoder-test PRIVATE Umicom::creative_workspace Umicom::platform)
        umicom_creative_configure_target(umicom-video-encoder-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-video-encoder-test)
        endif()
        add_test(NAME framework.video_sequence.encoder.actual COMMAND umicom-video-encoder-test "${UMICOM_VIDEO_FFMPEG_EXECUTABLE}")
        set_tests_properties(framework.video_sequence.encoder.actual PROPERTIES TIMEOUT 120 LABELS "framework;video-sequence;encoder;acceptance")
    endif()
endif()

# Lifetime regressions must not silently survive GTK ownership warnings.
if(BUILD_TESTING AND TARGET umicom-video-gtk-test)
    foreach(case cancel-open retire-dispatch retire-complete retire-refresh)
        set_tests_properties(framework.video_sequence.umicom-video-gtk-test.${case}
            PROPERTIES ENVIRONMENT "G_DEBUG=fatal-warnings")
    endforeach()
endif()
