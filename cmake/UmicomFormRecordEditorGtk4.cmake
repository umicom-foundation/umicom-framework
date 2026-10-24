# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Products share one native editor and the same portable form-record session.
if(NOT TARGET Umicom::structured_form_gtk4 OR NOT TARGET Umicom::form_records OR NOT TARGET Umicom::record_recovery)
    return()
endif()
include_guard(GLOBAL)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
include(GNUInstallDirs)
get_filename_component(_form_editor_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
add_library(umicom_form_record_editor_gtk4 STATIC "${_form_editor_root}/adapters/gtk4/form_record_editor_gtk4.c")
add_library(Umicom::form_record_editor_gtk4 ALIAS umicom_form_record_editor_gtk4)
set_target_properties(umicom_form_record_editor_gtk4 PROPERTIES EXPORT_NAME form_record_editor_gtk4
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom_form_record_editor_gtk4 PUBLIC Umicom::structured_form_gtk4 Umicom::form_records Umicom::record_recovery)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_form_record_editor_gtk4)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_form_record_editor_gtk4)
endif()
install(TARGETS umicom_form_record_editor_gtk4 EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
umicom_register_sdk_libraries(umicom_form_record_editor_gtk4)
install(FILES "${_form_editor_root}/include/umicom/ui/gtk4/form_record_editor.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_form_record_editor_gtk4 include/umicom/ui/gtk4/form_record_editor.h)
endif()
if(BUILD_TESTING)
    add_executable(umicom-form-record-editor-test "${_form_editor_root}/tests/data_history/test_form_record_editor_gtk4.c")
    target_link_libraries(umicom-form-record-editor-test PRIVATE Umicom::form_record_editor_gtk4)
    set_target_properties(umicom-form-record-editor-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-form-record-editor-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-form-record-editor-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-form-record-editor-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-form-record-editor-test FILES
            "${_form_editor_root}/tests/application_experience/structured_form_fixture.h")
    endif()
    set(_form_editor_cases options relative-path retired-actions)
    if(SQLite3_FOUND)
        list(APPEND _form_editor_cases keys missing new-discard required draft invalid-input invalid-attribution
            roundtrip busy later-edit dirty-open discard-saved stale read-later-edit retire-connect retire-read
            retire-save unroot-save acceptance-close acceptance-edit completion-close completion-edit
            prompt-context prompt-cancel prompt-accept input-attribution
            recovery-new recovery-saved recovery-exclusive recovery-dirty recovery-later-edit
            recovery-retire-write recovery-retire-read recovery-wrong-store recovery-namespace)
    endif()
    foreach(_form_editor_case IN LISTS _form_editor_cases)
        add_test(NAME framework.structured_forms.editor.gtk.${_form_editor_case}
            COMMAND umicom-form-record-editor-test ${_form_editor_case} "${CMAKE_CURRENT_BINARY_DIR}/form-editor-fixtures")
        set_tests_properties(framework.structured_forms.editor.gtk.${_form_editor_case} PROPERTIES
            TIMEOUT 45 SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings"
            LABELS "framework;forms;data;gtk4;lifetime;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.structured_forms.editor.gtk.${_form_editor_case} TARGETS umicom-form-record-editor-test)
        endif()
    endforeach()
endif()
unset(_form_editor_root)
unset(_form_editor_cases)
unset(_form_editor_case)
foreach(_form_editor_doc_component IN ITEMS Framework FrameworkSDK)
    set(_form_editor_doc_exclude "")
    if(_form_editor_doc_component STREQUAL "FrameworkSDK")
        set(_form_editor_doc_exclude EXCLUDE_FROM_ALL)
    endif()
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/guides/native-form-record-editor.html"
        "${CMAKE_CURRENT_LIST_DIR}/../docs/guides/synthetic-healthcare-profiles.html"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides"
        COMPONENT ${_form_editor_doc_component} ${_form_editor_doc_exclude})
endforeach()
unset(_form_editor_doc_component)
unset(_form_editor_doc_exclude)
