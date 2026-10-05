# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
# This composition depends on document ownership and language decoding. Neither
# lower-level service acquires a dependency on the other or on a native toolkit.
add_library(umicom_source_review STATIC "${CMAKE_CURRENT_LIST_DIR}/../src/source_review/workspace_edit.c")
add_library(Umicom::source_review ALIAS umicom_source_review)
set_target_properties(umicom_source_review PROPERTIES EXPORT_NAME source_review
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_source_review PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_source_review PUBLIC Umicom::document Umicom::developer)
umicom_apply_warnings(umicom_source_review)
umicom_apply_sanitizers(umicom_source_review)
install(TARGETS umicom_source_review EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
if(TARGET umicom_framework)
    target_link_libraries(umicom_framework INTERFACE Umicom::source_review)
endif()
if(BUILD_TESTING)
    add_executable(umicom-source-workspace-edit-review-test "${CMAKE_CURRENT_LIST_DIR}/../tests/source_review/test_workspace_edit.c")
    target_link_libraries(umicom-source-workspace-edit-review-test PRIVATE Umicom::source_review)
    umicom_apply_warnings(umicom-source-workspace-edit-review-test)
    umicom_apply_sanitizers(umicom-source-workspace-edit-review-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-source-workspace-edit-review-test)
    endif()
    foreach(case two-targets one-target reverse-order capture ownership unknown-target empty version version-unknown version-stale invalid-range overlap read-only-target read-only-dependency stale-before stale-target stale-dependency dependency-round-trip dependency-caret dependency-permission dependency-saved dependency-closed review-none review-partial review-repeat approval revision annotation annotation-missing annotation-revoke annotation-repeat annotation-unused annotation-optional annotation-noop annotation-shared unicode empty-replacement unchanged pending-typing undo redo active-tab new-document consumed arguments cancelled)
        add_test(NAME framework.source_review.workspace_edit.${case} COMMAND umicom-source-workspace-edit-review-test ${case})
        set_tests_properties(framework.source_review.workspace_edit.${case} PROPERTIES TIMEOUT 30 LABELS "framework;language;document;review;transaction;regression")
    endforeach()
endif()

if(TARGET umicom_ui_gtk4)
    target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::source_review)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWorkspaceEditReviewPanel.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWorkspaceRenamePanel.cmake")

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/complete-language-edits.html" DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/learning)

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWorkspaceActionsPanel.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWorkspaceActionPullPanel.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomWorkspaceActionFilterPanel.cmake")
