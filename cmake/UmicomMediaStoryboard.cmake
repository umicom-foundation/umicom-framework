#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomMediaStoryboard.cmake
#
# PURPOSE:
#   Register shared scene planning, native authoring and regression coverage.
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
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/media_storyboard.c")
if(TARGET umicom_creative_workspace_gtk4)
    target_sources(umicom_creative_workspace_gtk4 PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/media_storyboard_gtk4.c")
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ui/gtk4/media_storyboard.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
endif()
if(BUILD_TESTING)
    add_executable(umicom-media-storyboard-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/creative_workspace/test_media_storyboard.c")
    target_link_libraries(umicom-media-storyboard-test PRIVATE Umicom::creative_workspace)
    umicom_creative_configure_target(umicom-media-storyboard-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-media-storyboard-test)
    endif()
    foreach(case references immutable edits unknown malformed duplicate bounds unicode limits output)
        add_test(NAME framework.media_storyboard.${case} COMMAND umicom-media-storyboard-test ${case})
        set_tests_properties(framework.media_storyboard.${case} PROPERTIES
            TIMEOUT 20 LABELS "framework;creative;regression")
    endforeach()
    if(TARGET umicom_creative_workspace_gtk4)
        add_executable(umicom-media-storyboard-gtk-test
            "${CMAKE_CURRENT_LIST_DIR}/../tests/creative_workspace/test_media_storyboard_gtk4.c")
        target_link_libraries(umicom-media-storyboard-gtk-test PRIVATE Umicom::creative_workspace_gtk4)
        umicom_creative_configure_target(umicom-media-storyboard-gtk-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-media-storyboard-gtk-test)
        endif()
        foreach(case author reorder pending invalid handoff occupied retired composition source assets stale-assets)
            add_test(NAME framework.media_storyboard.gtk.${case} COMMAND umicom-media-storyboard-gtk-test ${case})
            set_tests_properties(framework.media_storyboard.gtk.${case} PROPERTIES
                TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;creative;gtk4;regression")
        endforeach()
    endif()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/media-storyboards.md"
    "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/media-storyboards.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Learning)
