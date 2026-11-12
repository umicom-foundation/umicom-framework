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
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/media_storyboard.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/media_storyboard_document.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/media_storyboard_file.c")
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

# Editable document coverage is portable; the wrapper fixture injects native
# I/O outcomes and does not replace the owner's real-file qualification.
if(BUILD_TESTING)
    add_executable(umicom-storyboard-document-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/creative_workspace/test_media_storyboard_document.c")
    add_executable(umicom-storyboard-file-contract-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/creative_workspace/test_media_storyboard_file_contract.c")
    foreach(_target IN ITEMS umicom-storyboard-document-test umicom-storyboard-file-contract-test)
        target_link_libraries("${_target}" PRIVATE Umicom::creative_workspace)
        umicom_creative_configure_target("${_target}")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("${_target}")
        endif()
    endforeach()
    foreach(_case IN ITEMS
        roundtrip
        empty
        unresolved
        incomplete-tag
        multiline
        unicode
        escaped
        max-scenes
        max-fields
        inactive-slots
        order
        bad-count
        unterminated-title
        unterminated-story
        draft-zero-duration
        draft-large-duration
        draft-invalid-utf8
        occupied-output
        cancel-export
        cancel-import
        null-arguments
        empty-input
        oversized-input
        excess-scenes
        raw-nul
        raw-invalid-utf8
        escaped-duplicate
        root-array
        root-null
        missing-scenes
        unknown-root
        duplicate-format
        future-version
        fractional-version
        wrong-format
        scenes-object
        scene-number
        unknown-scene
        duplicate-scene
        fractional-duration
        negative-duration
        zero-duration
        large-duration
        missing-story
        title-number
        blank-title
        blank-story
        surrogate
        decoded-nul
        control-story
        trailing-value
        late-invalid-scene
        writer-exact
        writer-overflow
        writer-sticky
        writer-null-text
    )
        add_test(NAME framework.media_storyboard.document.${_case}
            COMMAND umicom-storyboard-document-test "${_case}")
        set_tests_properties(framework.media_storyboard.document.${_case} PROPERTIES
            TIMEOUT 20 LABELS "framework;creative;document;regression")
    endforeach()
    foreach(_case IN ITEMS
        save-success
        save-failure
        save-cancel-before
        save-cancel-during
        save-failure-cancel-during
        save-invalid-draft
        save-relative-path
        load-success
        load-failure
        load-corrupt
        load-cancel-before
        load-cancel-after-read
        load-relative-path
        load-null-output
        load-empty
    )
        add_test(NAME framework.media_storyboard.file_contract.${_case}
            COMMAND umicom-storyboard-file-contract-test "${_case}")
        set_tests_properties(framework.media_storyboard.file_contract.${_case} PROPERTIES
            TIMEOUT 20 LABELS "framework;creative;injected-io;regression")
    endforeach()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/editable-storyboard-documents.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Learning)

if(BUILD_TESTING AND TARGET umicom_creative_workspace_gtk4)
    add_executable(umicom-storyboard-recovery-gtk-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/creative_workspace/test_media_storyboard_recovery_gtk4.c")
    target_link_libraries(umicom-storyboard-recovery-gtk-test PRIVATE Umicom::creative_workspace_gtk4)
    umicom_creative_configure_target(umicom-storyboard-recovery-gtk-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-storyboard-recovery-gtk-test)
    endif()
    foreach(_case IN ITEMS
        save-open
        pending-save
        pending-open
        save-approval
        open-approval
        path-change-reset
        edit-resets-approval
        damaged-open
        empty-open
        unicode
        stale-open
        busy-action
        retire-open
        save-replacement
        save-failure
        disabled-save
        disabled-open
    )
        add_test(NAME framework.media_storyboard.gtk.recovery.${_case}
            COMMAND umicom-storyboard-recovery-gtk-test "${_case}")
        set_tests_properties(framework.media_storyboard.gtk.recovery.${_case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;creative;gtk4;storage;regression")
    endforeach()
endif()
