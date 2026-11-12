#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomVideoWatermarkChecks.cmake
#
# PURPOSE:
#   Compose SVG watermark planning and owned-resource regression coverage.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
target_sources(umicom_creative_workspace PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/video_watermark.c")
if(TARGET umicom_creative_workspace_gtk4)
    target_sources(umicom_creative_workspace_gtk4 PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/media_watermark_gtk4.c")
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/branded-video-exports.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Framework)
if(BUILD_TESTING)
    umicom_video_cases(umicom-video-watermark-plan-test test_watermark_plan.c
        sequence soundtrack captions six-clips portrait small path-captured path-punctuation
        duplicate null-plan null-path relative parent-path capacity-path graph-capacity
        argument-capacity missing-map wrong-map missing-codec dangling-input invalid-count
        null-argument no-input unterminated-graph)
    if(TARGET umicom_creative_workspace_gtk4)
        add_executable(umicom-media-watermark-resource-test
            "${CMAKE_CURRENT_LIST_DIR}/../tests/video_sequence/test_watermark_resource.c")
        target_link_libraries(umicom-media-watermark-resource-test PRIVATE
            Umicom::creative_workspace_gtk4)
        umicom_creative_configure_target(umicom-media-watermark-resource-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-media-watermark-resource-test)
        endif()
        foreach(_case bytes unique fail-create fail-write fail-close fail-delete
                      missing-cleanup null-output occupied-output)
            add_test(NAME framework.video_sequence.watermark-resource.${_case}
                COMMAND umicom-media-watermark-resource-test "${_case}"
                "${CMAKE_CURRENT_LIST_DIR}/../resources/brand/umicom-logo-on-dark.svg")
            set_tests_properties(framework.video_sequence.watermark-resource.${_case}
                PROPERTIES TIMEOUT 30 LABELS "framework;video-sequence;branding;ownership;regression")
        endforeach()
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-video-watermark-plan-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/video_watermark.c"
            "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/video_render_internal.h")
        if(TARGET umicom-media-watermark-resource-test)
            umicom_target_qualification_inputs(umicom-media-watermark-resource-test FILES
                "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/media_watermark_gtk4.c"
                "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/media_workspace_style.c"
                "${CMAKE_CURRENT_LIST_DIR}/../resources/brand/umicom-logo-on-dark.svg")
        endif()
    endif()
endif()

# Actual codec qualification remains explicit and owner-run. A process-shaped
# fixture cannot establish that the canonical logo survives every video frame.
if(BUILD_TESTING AND UMICOM_VIDEO_FFMPEG_EXECUTABLE AND
   (WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Linux"))
    add_executable(umicom-video-watermark-encoder-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/video_sequence/test_watermark_encoder.c")
    target_link_libraries(umicom-video-watermark-encoder-test PRIVATE
        Umicom::creative_workspace Umicom::platform)
    umicom_creative_configure_target(umicom-video-watermark-encoder-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-video-watermark-encoder-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-video-watermark-encoder-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../tests/video_sequence/test_encoder.c"
            "${CMAKE_CURRENT_LIST_DIR}/../resources/brand/umicom-logo-on-dark.svg")
    endif()
    foreach(_case sequence soundtrack captions)
        add_test(NAME framework.video_sequence.watermark-encoder.${_case}
            COMMAND umicom-video-watermark-encoder-test "${_case}"
                "${UMICOM_VIDEO_FFMPEG_EXECUTABLE}"
                "${CMAKE_CURRENT_LIST_DIR}/../resources/brand/umicom-logo-on-dark.svg")
        set_tests_properties(framework.video_sequence.watermark-encoder.${_case}
            PROPERTIES TIMEOUT 120 LABELS "framework;video-sequence;encoder;branding;acceptance")
    endforeach()
endif()

install(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../examples/video_watermark/main.c"
    "${CMAKE_CURRENT_LIST_DIR}/../examples/video_watermark/CMakeLists.txt"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/examples/video_watermark" COMPONENT Learning)
if(BUILD_TESTING)
    add_executable(umicom-video-watermark-example
        "${CMAKE_CURRENT_LIST_DIR}/../examples/video_watermark/main.c")
    target_link_libraries(umicom-video-watermark-example PRIVATE Umicom::creative_workspace)
    umicom_creative_configure_target(umicom-video-watermark-example)
    # Creative-workspace builds can omit the general CLI tools, so load their
    # shared identity helper explicitly rather than relying on root include order.
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomApplicationBranding.cmake")
    if(WIN32)
        enable_language(RC)
    endif()
    umicom_apply_application_branding(TARGET umicom-video-watermark-example
        PRODUCT_NAME "Umicom Video Watermark Example" INTERNAL_NAME "umicom-video-watermark-example"
        RESOURCE_ROOT "${CMAKE_CURRENT_LIST_DIR}/../resources")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-video-watermark-example)
    endif()
    foreach(_case self-check reject-relative duplicate)
        add_test(NAME framework.video_sequence.watermark-example.${_case}
            COMMAND umicom-video-watermark-example "${_case}")
        set_tests_properties(framework.video_sequence.watermark-example.${_case}
            PROPERTIES TIMEOUT 30 LABELS "framework;video-sequence;learning;no-process;no-filesystem")
    endforeach()
endif()
