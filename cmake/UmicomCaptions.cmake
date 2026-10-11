# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Extend the canonical creative owner so native and future web/mobile adapters
# share caption parsing, immutable edits and portable output.
include_guard(GLOBAL)
target_sources(umicom_creative_workspace PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/captions.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/captions_srt.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/captions_edit.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/captions_export.c")
if(TARGET umicom_creative_workspace_gtk4)
    target_sources(umicom_creative_workspace_gtk4 PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/captions_gtk4.c")
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ui/gtk4/captions.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
endif()
if(BUILD_TESTING)
    add_executable(umicom-captions-test "${CMAKE_CURRENT_LIST_DIR}/../tests/creative_workspace/test_captions.c")
    target_link_libraries(umicom-captions-test PRIVATE Umicom::creative_workspace)
    umicom_creative_configure_target(umicom-captions-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-captions-test)
    endif()
    foreach(case import crlf bom empty overlap identifiers invalid-time invalid-order invalid-number empty-cue lone-cr utf8 markup null-byte text-capacity cue-capacity document-capacity shift-forward shift-backward shift-underflow shift-overflow shift-intmin clip-rebase clip-original clip-empty replace insert remove edit-order export-srt export-vtt cancel live-output read-bounds)
        add_test(NAME "framework.captions.${case}" COMMAND umicom-captions-test "${case}")
        set_tests_properties("framework.captions.${case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;creative;captions;regression")
    endforeach()
    if(TARGET umicom_creative_workspace_gtk4)
        add_executable(umicom-captions-gtk-test "${CMAKE_CURRENT_LIST_DIR}/../tests/creative_workspace/test_captions_gtk4.c")
        target_link_libraries(umicom-captions-gtk-test PRIVATE Umicom::creative_workspace_gtk4)
        umicom_creative_configure_target(umicom-captions-gtk-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-captions-gtk-test)
        endif()
        add_executable(umicom-caption-files-gtk-test "${CMAKE_CURRENT_LIST_DIR}/../tests/creative_workspace/test_caption_files_gtk4.c")
        target_link_libraries(umicom-caption-files-gtk-test PRIVATE Umicom::creative_workspace_gtk4)
        umicom_creative_configure_target(umicom-caption-files-gtk-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-caption-files-gtk-test)
        endif()
        foreach(case open open-invalid open-stale open-cancel save-srt save-vtt save-existing)
            add_test(NAME "framework.captions.files.${case}" COMMAND umicom-caption-files-gtk-test "${case}")
            set_tests_properties("framework.captions.files.${case}" PROPERTIES
                TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;creative;captions;gtk4;file")
        endforeach()
        foreach(case prepare preview overlap shift shift-rejected clip undo invalidate invalid-draft open-denied retained reentrant media music)
            add_test(NAME "framework.captions.gtk.${case}" COMMAND umicom-captions-gtk-test "${case}")
            set_tests_properties("framework.captions.gtk.${case}" PROPERTIES
                TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;creative;captions;gtk4;regression")
        endforeach()
    endif()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/caption-editing.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Learning)
