# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-workspace-edit-catalogue-test "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_workspace_edit_catalogue.c")
    target_link_libraries(umicom-workspace-edit-catalogue-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-workspace-edit-catalogue-test)
    umicom_apply_sanitizers(umicom-workspace-edit-catalogue-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-workspace-edit-catalogue-test)
    endif()
    foreach(case changes null empty empty-map empty-array empty-edits version zero-version negative-version minimum-version null-version prefer-document-changes annotation annotation-no-confirmation unused-annotation missing-annotation confirmation-type annotation-label-missing annotation-description-type map-annotation version-type version-fraction version-overflow version-missing repeated-document resource-create resource-rename resource-delete changes-type documents-type annotations-type edits-type bad-uri text-type negative-range reversed-range unknown-edit-shape two-documents duplicate-uri duplicate-member duplicate-annotation owned response wrong-id error legacy-changes legacy-version legacy-annotation legacy-partial document-limit edit-limit annotation-limit cancelled arguments)
        add_test(NAME framework.language_runtime.workspace_edit_catalogue.${case} COMMAND umicom-workspace-edit-catalogue-test ${case})
        set_tests_properties(framework.language_runtime.workspace_edit_catalogue.${case} PROPERTIES TIMEOUT 30
            LABELS "framework;language;workspace-edits;ownership;regression")
    endforeach()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/workspace-edit-review.html" DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/docs/learning")
