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

# Scene planning shares these source and provider handoffs rather than adding
# an independent generation owner in the Media application.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomMediaStoryboard.cmake")


# Recovery stores editing state separately from prepared model requests.
# Reuse the shared JSON and file services; native clients remain thin owners.
target_sources(umicom_creative_workspace PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/media_brief_document.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/creative_workspace/media_brief_file.c")
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/creative_workspace/media_brief_document.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/creative_workspace" COMPONENT Framework)
if(BUILD_TESTING)
    add_executable(umicom-creation-document-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/creative_workspace/test_media_brief_document.c")
    add_executable(umicom-creation-file-contract-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/creative_workspace/test_media_brief_file_contract.c")
    foreach(_target IN ITEMS umicom-creation-document-test umicom-creation-file-contract-test)
        target_link_libraries("${_target}" PRIVATE Umicom::creative_workspace)
        umicom_creative_configure_target("${_target}")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("${_target}")
        endif()
    endforeach()
    # Including production wrappers must not introduce fixture-local shadowing.
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(umicom-creation-file-contract-test PRIVATE -Werror=shadow)
    endif()
    foreach(_case IN ITEMS
        roundtrip
        kinds
        empty
        prompt-only
        stale-prompt
        whitespace
        unicode
        escaped
        max-fields
        ignored-tail
        invalid-kind
        unterminated-fields
        malformed-fields
        control-fields
        occupied-output
        cancel-export
        cancel-import
        null-arguments
        empty-input
        oversized-input
        raw-nul
        raw-invalid-utf8
        oversized-prompt
        root-array
        root-null
        missing-prompt
        unknown-root
        duplicate-root
        escaped-duplicate
        future-version
        fractional-version
        wrong-format
        draft-array
        unknown-draft
        missing-direction
        kind-number
        kind-unknown
        kind-case
        duplicate-source
        title-number
        source-null
        prompt-array
        decoded-nul
        prompt-control
        surrogate
        trailing-value
    )
        add_test(NAME framework.media_brief.document.${_case}
            COMMAND umicom-creation-document-test "${_case}")
        set_tests_properties(framework.media_brief.document.${_case} PROPERTIES
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
        add_test(NAME framework.media_brief.file_contract.${_case}
            COMMAND umicom-creation-file-contract-test "${_case}")
        set_tests_properties(framework.media_brief.file_contract.${_case} PROPERTIES
            TIMEOUT 20 LABELS "framework;creative;injected-io;regression")
    endforeach()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/editable-creation-drafts.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Learning)


if(BUILD_TESTING AND TARGET umicom_creative_workspace_gtk4)
    add_executable(umicom-creation-recovery-gtk-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/creative_workspace/test_media_brief_recovery_gtk4.c")
    target_link_libraries(umicom-creation-recovery-gtk-test PRIVATE Umicom::creative_workspace_gtk4)
    umicom_creative_configure_target(umicom-creation-recovery-gtk-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-creation-recovery-gtk-test)
    endif()
    foreach(_case IN ITEMS
        save-open
        save-incomplete
        save-stale
        load-empty
        prompt-only
        unicode
        save-approval
        open-approval
        path-reset
        source-reset
        prompt-reset
        kind-reset
        damaged-open
        retire-open
        stale-source
        stale-prompt
        stale-path
        busy-set-source
        disabled-save
        disabled-open
        save-failure
        cancel-open
        oversized-save
        prepare-protection
    )
        add_test(NAME framework.media_brief.gtk.recovery.${_case}
            COMMAND umicom-creation-recovery-gtk-test "${_case}")
        set_tests_properties(framework.media_brief.gtk.recovery.${_case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;creative;gtk4;storage;regression")
    endforeach()
endif()
