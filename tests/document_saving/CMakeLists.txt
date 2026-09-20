#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/document_saving/CMakeLists.txt
# PURPOSE: Verify Save All against the production document implementation.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
add_executable(umicom-document-saving-test test_save_session.c)
target_link_libraries(umicom-document-saving-test PRIVATE Umicom::document)
umicom_apply_warnings(umicom-document-saving-test)
umicom_apply_sanitizers(umicom-document-saving-test)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-document-saving-test)
endif()
# Previous registration list retained for reference; every original case
# remains in the expanded list, with five cancellation/progress cases added.
# foreach(case IN ITEMS invalid-api empty clean named untitled mixed cancel-before
#     cancel-after closed renamed already-saved readonly external-conflict
#     new-document other-tab bad-path other-destination current-draft history-target partial-failure waiting-closed waiting-renamed waiting-other-tab legacy-preflight)
foreach(case IN ITEMS cancel-waiting-path cancel-retry cancel-complete cancel-failed progress-copy
    invalid-api empty clean named untitled mixed cancel-before
    cancel-after closed renamed already-saved readonly external-conflict
    new-document other-tab bad-path other-destination current-draft history-target partial-failure waiting-closed waiting-renamed waiting-other-tab legacy-preflight)
    add_test(NAME framework.document_saving.${case} COMMAND umicom-document-saving-test ${case})
    set_tests_properties(framework.document_saving.${case} PROPERTIES TIMEOUT 30
        LABELS "framework;document;saving;regression")
endforeach()

# Compile the canonical teaching example instead of testing a copied snippet.
add_executable(umicom-document-save-all-example ../../examples/editor_workflow/save_all.c)
target_link_libraries(umicom-document-save-all-example PRIVATE Umicom::document)
umicom_apply_warnings(umicom-document-save-all-example)
umicom_apply_sanitizers(umicom-document-save-all-example)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-document-save-all-example)
endif()
add_test(NAME framework.document_saving.example COMMAND umicom-document-save-all-example)
set_tests_properties(framework.document_saving.example PROPERTIES TIMEOUT 30 LABELS "framework;document;saving;example")
install(TARGETS umicom-document-save-all-example RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(TARGET umicom_ui_gtk4)
    add_executable(umicom-document-saving-gtk4-test test_save_all_gtk4.c)
    target_link_libraries(umicom-document-saving-gtk4-test PRIVATE Umicom::ui_gtk4 Umicom::document)
    umicom_apply_warnings(umicom-document-saving-gtk4-test)
    umicom_apply_sanitizers(umicom-document-saving-gtk4-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-document-saving-gtk4-test)
    endif()
    # Previous list retained: the expanded registration keeps all five cases.
    # foreach(case empty named cancel unbind complete-unbind)
    foreach(case empty named cancel unbind complete-unbind progress paste-busy repeat-cancel)
        add_test(NAME framework.document_saving.gtk4.${case} COMMAND umicom-document-saving-gtk4-test ${case})
        set_tests_properties(framework.document_saving.gtk4.${case} PROPERTIES TIMEOUT 30
            SKIP_RETURN_CODE 77 RUN_SERIAL TRUE LABELS "framework;document;saving;gtk4;native")
    endforeach()
endif()

# Progress text is produced by the same library used by Studio's completion.
add_executable(umicom-document-save-progress-test test_save_progress.c)
target_link_libraries(umicom-document-save-progress-test PRIVATE Umicom::document)
umicom_apply_warnings(umicom-document-save-progress-test)
umicom_apply_sanitizers(umicom-document-save-progress-test)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-document-save-progress-test)
endif()
foreach(case invalid name counts phase capacity phases immutable)
    add_test(NAME framework.document_saving.report.${case}
        COMMAND umicom-document-save-progress-test ${case})
    set_tests_properties(framework.document_saving.report.${case} PROPERTIES
        TIMEOUT 30 LABELS "framework;document;saving;regression")
endforeach()

# Link the real public functions from their owning libraries. Header-only type
# checks cannot prove that an archive contains a function definition.
add_executable(umicom-document-save-link-check
    ../../examples/editor_workflow/save_progress.c)
target_link_libraries(umicom-document-save-link-check PRIVATE Umicom::document)
umicom_apply_warnings(umicom-document-save-link-check)
umicom_apply_sanitizers(umicom-document-save-link-check)
add_test(NAME framework.document_saving.link.core COMMAND umicom-document-save-link-check)
set_tests_properties(framework.document_saving.link.core PROPERTIES
    TIMEOUT 30 LABELS "framework;document;saving;link;example")

add_executable(umicom-save-progress-validation-test test_save_progress_validation.c)
target_link_libraries(umicom-save-progress-validation-test PRIVATE Umicom::document)
umicom_apply_warnings(umicom-save-progress-validation-test)
umicom_apply_sanitizers(umicom-save-progress-validation-test)
foreach(case null partition overflow ready-empty path-empty unknown-status host-failure
        counts-visible name immutable all-phases)
    add_test(NAME framework.document_saving.validation.${case}
        COMMAND umicom-save-progress-validation-test ${case})
    set_tests_properties(framework.document_saving.validation.${case} PROPERTIES
        TIMEOUT 30 LABELS "framework;document;saving;regression")
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-document-save-link-check)
    umicom_register_validation_target(umicom-save-progress-validation-test)
endif()

if(TARGET umicom_ui_gtk4)
    add_executable(umicom-document-save-native-link-check test_save_link_gtk4.c)
    target_link_libraries(umicom-document-save-native-link-check PRIVATE
        Umicom::ui_gtk4 Umicom::document)
    umicom_apply_warnings(umicom-document-save-native-link-check)
    umicom_apply_sanitizers(umicom-document-save-native-link-check)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-document-save-native-link-check)
    endif()
    # No gtk_init and no display: this is a linkage/argument test, not a click test.
    add_test(NAME framework.document_saving.link.gtk4
        COMMAND umicom-document-save-native-link-check)
    set_tests_properties(framework.document_saving.link.gtk4 PROPERTIES
        TIMEOUT 30 LABELS "framework;document;saving;link;gtk4-library")
    add_custom_target(umicom-save-link-readiness)
    add_dependencies(umicom-save-link-readiness
        umicom-document-save-link-check umicom-document-save-native-link-check
        umicom-document-save-all-example umicom-document-saving-gtk4-test)
else()
    # Keep the command discoverable, but do not claim the native symbol was linked.
    add_custom_target(umicom-save-link-readiness
        COMMAND "${CMAKE_COMMAND}" -E echo
            "Save link readiness requires the GTK4 adapter and BUILD_TESTING=ON."
        COMMAND "${CMAKE_COMMAND}" -E false VERBATIM)
endif()
