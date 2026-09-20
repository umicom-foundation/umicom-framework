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
foreach(case IN ITEMS invalid-api empty clean named untitled mixed cancel-before
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
    foreach(case empty named cancel unbind complete-unbind)
        add_test(NAME framework.document_saving.gtk4.${case} COMMAND umicom-document-saving-gtk4-test ${case})
        set_tests_properties(framework.document_saving.gtk4.${case} PROPERTIES TIMEOUT 30
            SKIP_RETURN_CODE 77 RUN_SERIAL TRUE LABELS "framework;document;saving;gtk4;native")
    endforeach()
endif()
