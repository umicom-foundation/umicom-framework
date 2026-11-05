#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomMediaBrief.cmake
#
# PURPOSE:
#   Register portable creation briefs, native handoffs and regression coverage.
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
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/media_brief.c")
if(TARGET umicom_creative_workspace_gtk4)
    target_sources(umicom_creative_workspace_gtk4 PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/media_brief_gtk4.c")
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ui/gtk4/media_brief.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
endif()
if(BUILD_TESTING)
    add_executable(umicom-media-brief-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/creative_workspace/test_media_brief.c")
    target_link_libraries(umicom-media-brief-test PRIVATE Umicom::creative_workspace)
    umicom_creative_configure_target(umicom-media-brief-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-media-brief-test)
    endif()
    foreach(case kinds immutable blank control malformed unterminated limits)
        add_test(NAME framework.media_brief.${case} COMMAND umicom-media-brief-test ${case})
        set_tests_properties(framework.media_brief.${case} PROPERTIES
            TIMEOUT 20 LABELS "framework;creative;regression")
    endforeach()
    if(TARGET umicom_creative_workspace_gtk4)
        add_executable(umicom-media-brief-gtk-test
            "${CMAKE_CURRENT_LIST_DIR}/../tests/creative_workspace/test_media_brief_gtk4.c")
        target_link_libraries(umicom-media-brief-gtk-test PRIVATE Umicom::creative_workspace_gtk4)
        umicom_creative_configure_target(umicom-media-brief-gtk-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-media-brief-gtk-test)
        endif()
        foreach(case prepare stale stale-export edited replace image video invalid-handoff oversized-brief model-path retired missing composition)
            add_test(NAME framework.media_brief.gtk.${case} COMMAND umicom-media-brief-gtk-test ${case})
            set_tests_properties(framework.media_brief.gtk.${case} PROPERTIES
                TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;creative;gtk4;regression")
        endforeach()
    endif()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/media-creation-briefs.html"
    "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/media-creation-briefs.md"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Learning)
