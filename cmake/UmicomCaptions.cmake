# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Extend the canonical creative owner so native and future web/mobile adapters
# share caption parsing, immutable edits and portable output.
include_guard(GLOBAL)
target_sources(umicom_creative_workspace PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/captions.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/captions_srt.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/captions_webvtt.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/captions_edit.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/captions_rows.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/captions_export.c")
if(TARGET umicom_creative_workspace_gtk4)
    target_sources(umicom_creative_workspace_gtk4 PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/captions_gtk4.c")
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ui/gtk4/captions.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
endif()
if(BUILD_TESTING)

    # Exercise portable interchange independently of GTK and provider services.
    add_executable(umicom-caption-webvtt-test "${CMAKE_CURRENT_LIST_DIR}/../tests/creative_workspace/test_caption_webvtt.c")
    target_link_libraries(umicom-caption-webvtt-test PRIVATE Umicom::creative_workspace)
    umicom_creative_configure_target(umicom-caption-webvtt-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-caption-webvtt-test)
    endif()
    foreach(case import auto-vtt auto-srt short-time numeric-label crlf bom long-hour empty roundtrip null-byte text-capacity cue-capacity document-capacity live-output arguments header header-suffix header-text header-gap metadata note style region named-label duplicate-label zero-label label-overflow settings markup entity cr utf8 minute second hour millisecond arrow duration order empty-cue incomplete)
        add_test(NAME "framework.captions.webvtt.${case}" COMMAND umicom-caption-webvtt-test "${case}")
        set_tests_properties("framework.captions.webvtt.${case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;creative;captions;regression")
    endforeach()
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
        foreach(case open-vtt open-vtt-invalid)
            add_test(NAME "framework.captions.files.${case}" COMMAND umicom-caption-files-gtk-test "${case}")
            set_tests_properties("framework.captions.files.${case}" PROPERTIES
                TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;creative;captions;gtk4;file")
        endforeach()
        foreach(case prepare-vtt invalid-vtt)
            add_test(NAME "framework.captions.gtk.${case}" COMMAND umicom-captions-gtk-test "${case}")
            set_tests_properties("framework.captions.gtk.${case}" PROPERTIES
                TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;creative;captions;gtk4;regression")
        endforeach()
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

# Row editing stays in the portable caption owner; GTK only gathers a draft.
if(BUILD_TESTING)
    add_executable(umicom-caption-rows-test "${CMAKE_CURRENT_LIST_DIR}/../tests/creative_workspace/test_caption_rows.c")
    target_link_libraries(umicom-caption-rows-test PRIVATE Umicom::creative_workspace)
    umicom_creative_configure_target(umicom-caption-rows-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-caption-rows-test)
    endif()
    foreach(case split equal-start split-boundary split-order split-text merge merge-gap merge-enclosed merge-capacity cue-capacity arguments)
        add_test(NAME "framework.captions.rows.${case}" COMMAND umicom-caption-rows-test "${case}")
        set_tests_properties("framework.captions.rows.${case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;creative;captions;regression")
    endforeach()
    if(TARGET umicom_creative_workspace_gtk4)
        add_executable(umicom-caption-rows-gtk-test "${CMAKE_CURRENT_LIST_DIR}/../tests/creative_workspace/test_caption_rows_gtk4.c")
        target_link_libraries(umicom-caption-rows-gtk-test PRIVATE Umicom::creative_workspace_gtk4)
        umicom_creative_configure_target(umicom-caption-rows-gtk-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-caption-rows-gtk-test)
        endif()
        foreach(case load replace insert remove split merge undo invalid stale row-selection draft-change reentrant retained)
            add_test(NAME "framework.captions.rows.gtk.${case}" COMMAND umicom-caption-rows-gtk-test "${case}")
            set_tests_properties("framework.captions.rows.gtk.${case}" PROPERTIES
                TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;creative;captions;gtk4;regression")
        endforeach()
    endif()
endif()
