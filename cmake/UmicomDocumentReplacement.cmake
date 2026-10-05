# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Review tests use the real coordinator and its existing history transaction.
include_guard(GLOBAL)
# The sequence uses the existing document owner; consumers link no extra store.
target_sources(umicom_document PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/document/replacement_session.c")
if(BUILD_TESTING)
    foreach(group capture apply stale limits)
        add_executable(umicom-document-replacement-${group}-test
            "${CMAKE_CURRENT_LIST_DIR}/../tests/document/test_replacement_${group}.c")
        target_link_libraries(umicom-document-replacement-${group}-test PRIVATE Umicom::Framework)
        umicom_apply_warnings(umicom-document-replacement-${group}-test)
        umicom_apply_sanitizers(umicom-document-replacement-${group}-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-document-replacement-${group}-test)
        endif()
    endforeach()
    foreach(case draft owned-inputs case unicode overlap delete literal no-match empty unchanged invalid-output)
        add_test(NAME framework.document.replacement.capture.${case} COMMAND umicom-document-replacement-capture-test ${case})
        set_tests_properties(framework.document.replacement.capture.${case} PROPERTIES TIMEOUT 45 LABELS "document;replacement;regression")
    endforeach()
    foreach(case undo-redo pending-typing inactive caret consumed no-change second-plan)
        add_test(NAME framework.document.replacement.apply.${case} COMMAND umicom-document-replacement-apply-test ${case})
        set_tests_properties(framework.document.replacement.apply.${case} PROPERTIES TIMEOUT 45 LABELS "document;replacement;regression")
    endforeach()
    foreach(case draft round-trip store saved-path marker read-only closed other-owner uri view-identity selection-allowed)
        add_test(NAME framework.document.replacement.stale.${case} COMMAND umicom-document-replacement-stale-test ${case})
        set_tests_properties(framework.document.replacement.stale.${case} PROPERTIES TIMEOUT 45 LABELS "document;replacement;regression")
    endforeach()
    foreach(case full-text maximum growth invalid-utf8 invalid-draft input-limit)
        add_test(NAME framework.document.replacement.limits.${case} COMMAND umicom-document-replacement-limits-test ${case})
        set_tests_properties(framework.document.replacement.limits.${case} PROPERTIES TIMEOUT 60 LABELS "document;replacement;regression")
    endforeach()
endif()
# Match existing Linux linker fault-injection support; no Windows claim.
if(BUILD_TESTING AND CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_executable(umicom-document-replacement-allocation-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/document/test_replacement_allocations.c")
    target_link_libraries(umicom-document-replacement-allocation-test PRIVATE Umicom::Framework)
    target_link_options(umicom-document-replacement-allocation-test PRIVATE
        -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc)
    umicom_apply_warnings(umicom-document-replacement-allocation-test)
    umicom_apply_sanitizers(umicom-document-replacement-allocation-test)
    foreach(case prepare apply)
        add_test(NAME framework.document.replacement.allocation.${case}
            COMMAND umicom-document-replacement-allocation-test ${case})
        set_tests_properties(framework.document.replacement.allocation.${case} PROPERTIES
            TIMEOUT 60 LABELS "document;replacement;fault-injection")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-document-replacement-allocation-test)
    endif()
endif()
if(UMICOM_BUILD_LEARNING_EXAMPLES)
    add_executable(umicom-replacement-review-example
        "${CMAKE_CURRENT_LIST_DIR}/../examples/editor_workflow/replacement_review.c")
    target_link_libraries(umicom-replacement-review-example PRIVATE Umicom::Framework)
    umicom_apply_warnings(umicom-replacement-review-example)
    umicom_apply_sanitizers(umicom-replacement-review-example)
    if(BUILD_TESTING)
        add_test(NAME framework.document.replacement.lesson COMMAND umicom-replacement-review-example)
        set_tests_properties(framework.document.replacement.lesson PROPERTIES TIMEOUT 30 LABELS "document;replacement;example")
    endif()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/REVIEWED_DOCUMENT_REPLACEMENT.md"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom-framework/docs)

if(BUILD_TESTING AND TARGET umicom_ui_gtk4)
    add_executable(umicom-window-lifecycle-gtk4-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ui/test_window_lifecycle_gtk4.c")
    target_link_libraries(umicom-window-lifecycle-gtk4-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-window-lifecycle-gtk4-test)
    umicom_apply_sanitizers(umicom-window-lifecycle-gtk4-test)
    foreach(case retained already-closed receiver-gone reentrant invalid)
        add_test(NAME framework.gtk4.window_lifecycle.${case} COMMAND umicom-window-lifecycle-gtk4-test ${case})
        set_tests_properties(framework.gtk4.window_lifecycle.${case} PROPERTIES
            SKIP_RETURN_CODE 77 TIMEOUT 30 LABELS "gtk4;lifetime;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-window-lifecycle-gtk4-test)
    endif()
endif()

# Sequence coverage uses independent drafts and the production history owner.
if(BUILD_TESTING)
    add_executable(umicom-document-replacement-session-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/document/test_replacement_session.c")
    target_link_libraries(umicom-document-replacement-session-test PRIVATE Umicom::Framework)
    umicom_apply_warnings(umicom-document-replacement-session-test)
    umicom_apply_sanitizers(umicom-document-replacement-session-test)
    foreach(case apply skip empty invalid owned-inputs no-match same delete late-open closed read-only pinned
            cancel-ready invalid-decision stale skip-stale undo cancel-partial fresh-turn
            path-before-turn path-after-capture close-question read-only-question partial-failure)
        add_test(NAME framework.document.replacement.session.${case} COMMAND umicom-document-replacement-session-test ${case})
        set_tests_properties(framework.document.replacement.session.${case} PROPERTIES
            TIMEOUT 45 LABELS "document;replacement;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-document-replacement-session-test)
    endif()
endif()

if(BUILD_TESTING AND TARGET umicom_ui_gtk4)
    add_executable(umicom-document-replacement-session-native-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/document/test_replacement_session_gtk4.c")
    target_link_libraries(umicom-document-replacement-session-native-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-document-replacement-session-native-test)
    umicom_apply_sanitizers(umicom-document-replacement-session-native-test)
    # Parent teardown and creation observers exercise native reentrancy too.
    foreach(case apply skip stop stale destroy unbind pending-close complete-unbind busy creation-unbind parent-close)
        add_test(NAME framework.document.replacement.session.gtk4.${case}
            COMMAND umicom-document-replacement-session-native-test ${case})
        set_tests_properties(framework.document.replacement.session.gtk4.${case} PROPERTIES
            TIMEOUT 45 SKIP_RETURN_CODE 77 LABELS "gtk4;document;replacement;lifetime;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-document-replacement-session-native-test)
    endif()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/REVIEW_OPEN_DOCUMENT_REPLACEMENTS.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom-framework/docs)

# Native notifications must never redirect an immutable comparison to borrowed
# storage changed halfway through constructing its two text panes.
if(BUILD_TESTING AND TARGET umicom_ui_gtk4)
    add_executable(umicom-comparison-construction-native-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_productivity/test_comparison_construction_gtk4.c")
    target_link_libraries(umicom-comparison-construction-native-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-comparison-construction-native-test)
    umicom_apply_sanitizers(umicom-comparison-construction-native-test)
    add_test(NAME framework.gtk4.comparison.construction COMMAND umicom-comparison-construction-native-test)
    set_tests_properties(framework.gtk4.comparison.construction PROPERTIES
        TIMEOUT 45 SKIP_RETURN_CODE 77 LABELS "gtk4;comparison;lifetime;regression")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-comparison-construction-native-test)
    endif()
endif()


# Selected proposals exercise the existing document and Undo owners with local fixtures.
if(BUILD_TESTING)
    add_executable(umicom-document-proposal-test "${CMAKE_CURRENT_LIST_DIR}/../tests/document/test_proposal.c")
    target_link_libraries(umicom-document-proposal-test PRIVATE Umicom::Framework)
    umicom_apply_warnings(umicom-document-proposal-test)
    umicom_apply_sanitizers(umicom-document-proposal-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-document-proposal-test)
    endif()
    foreach(case capture apply pending-typing owned delete unchanged approval revision invalid stale round-trip read-only closed other-owner other-tab caret unicode large limits arguments)
        add_test(NAME framework.document.proposal.${case} COMMAND umicom-document-proposal-test ${case})
        set_tests_properties(framework.document.proposal.${case} PROPERTIES TIMEOUT 45 LABELS "framework;document;proposal;review;regression")
    endforeach()
endif()

if(BUILD_TESTING AND TARGET umicom_ui_gtk4)
    add_executable(umicom-document-proposal-native-test "${CMAKE_CURRENT_LIST_DIR}/../tests/document/test_proposal_gtk4.c")
    target_link_libraries(umicom-document-proposal-native-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-document-proposal-native-test)
    umicom_apply_sanitizers(umicom-document-proposal-native-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-document-proposal-native-test)
    endif()
    foreach(case apply undo no-preview no-approval edit-after-preview stale cancel retained unbind parent-close creation-unbind preview-unbind preview-edit completion-unbind busy private delete)
        add_test(NAME framework.document.proposal.gtk4.${case} COMMAND umicom-document-proposal-native-test ${case})
        set_tests_properties(framework.document.proposal.gtk4.${case} PROPERTIES TIMEOUT 45 SKIP_RETURN_CODE 77 LABELS "framework;document;proposal;gtk4;lifetime;regression")
    endforeach()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/review-selected-code-replacements.html" DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
