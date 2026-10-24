# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Products share one native editor and the same portable form-record session.
if(NOT TARGET Umicom::structured_form_gtk4 OR NOT TARGET Umicom::form_records OR NOT TARGET Umicom::record_recovery OR NOT TARGET Umicom::form_inspect_gtk4)
    return()
endif()
# The shared file service is required for the editor's explicit history export.
if(NOT TARGET Umicom::record_bundle_files)
    return()
endif()
# The native query editor is shared with other hosts.
if(NOT TARGET Umicom::structured_form_filter_gtk4)
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
target_link_libraries(umicom_form_record_editor_gtk4 PUBLIC Umicom::structured_form_gtk4 Umicom::form_records Umicom::record_recovery Umicom::form_inspect_gtk4)
target_link_libraries(umicom_form_record_editor_gtk4 PUBLIC Umicom::record_bundle_files)
target_link_libraries(umicom_form_record_editor_gtk4 PUBLIC Umicom::structured_form_filter_gtk4)
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
        list(APPEND _form_editor_cases button-save zero-time keys missing new-discard required draft invalid-input invalid-attribution
            roundtrip busy later-edit dirty-open discard-saved stale read-later-edit retire-connect retire-read
            retire-save unroot-save acceptance-close acceptance-edit completion-close completion-edit
            prompt-context prompt-cancel prompt-accept input-attribution
            recovery-new recovery-saved recovery-exclusive recovery-dirty recovery-later-edit
            recovery-retire-write recovery-retire-read recovery-wrong-store recovery-namespace
            review-new review-unresolved review-local review-incoming review-base review-later-read
            review-later-apply review-retire review-stale-save review-retained-choice review-unit review-disjoint
            catalogue-empty catalogue-open catalogue-dirty catalogue-late-query catalogue-stale catalogue-late-open catalogue-pages catalogue-changed-page history-inspect history-older history-stale-page history-retire inspection-retire inspection-schema history-late-edit history-record-change)
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
# Separate versioned-history scenarios require both a display and SQLite.
if(BUILD_TESTING AND SQLite3_FOUND)
    add_executable(umicom-form-record-versions-test "${_form_editor_root}/tests/data_history/test_form_record_versions_gtk4.c")
    target_link_libraries(umicom-form-record-versions-test PRIVATE Umicom::form_record_editor_gtk4)
    set_target_properties(umicom-form-record-versions-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-form-record-versions-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-form-record-versions-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-form-record-versions-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-form-record-versions-test FILES
            "${_form_editor_root}/tests/application_experience/structured_form_fixture.h")
    endif()
    foreach(_form_version_case IN ITEMS historical unknown fallback dirty repeat missing-active clone-active late retired retire-read)
        add_test(NAME framework.structured_forms.editor.versions.${_form_version_case}
            COMMAND umicom-form-record-versions-test ${_form_version_case} "${CMAKE_CURRENT_BINARY_DIR}/form-editor-version-fixtures")
        set_tests_properties(framework.structured_forms.editor.versions.${_form_version_case} PROPERTIES TIMEOUT 45
            SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings"
            LABELS "framework;forms;data;gtk4;schema-catalogue;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.structured_forms.editor.versions.${_form_version_case} TARGETS umicom-form-record-versions-test)
        endif()
    endforeach()
    unset(_form_version_case)
endif()
# Async exports retain their admitted identity and complete after window retirement.
if(BUILD_TESTING AND SQLite3_FOUND)
    add_executable(umicom-form-record-export-test "${_form_editor_root}/tests/data_history/test_form_record_export_gtk4.c")
    target_link_libraries(umicom-form-record-export-test PRIVATE Umicom::form_record_editor_gtk4)
    set_target_properties(umicom-form-record-export-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-form-record-export-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-form-record-export-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-form-record-export-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-form-record-export-test FILES
            "${_form_editor_root}/tests/application_experience/structured_form_fixture.h")
    endif()
    foreach(_form_export_case IN ITEMS roundtrip buttons dirty unsaved empty disconnected relative existing stale retire-write dispose-write later-edit later-path busy capacity missing-parent)
        add_test(NAME framework.structured_forms.export.gtk.${_form_export_case}
            COMMAND umicom-form-record-export-test ${_form_export_case} "${CMAKE_CURRENT_BINARY_DIR}/form-export-fixtures")
        set_tests_properties(framework.structured_forms.export.gtk.${_form_export_case} PROPERTIES TIMEOUT 45
            SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings" LABELS "framework;forms;data;gtk4;export;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.structured_forms.export.gtk.${_form_export_case} TARGETS umicom-form-record-export-test)
        endif()
    endforeach()
endif()
unset(_form_export_case)
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
        "${CMAKE_CURRENT_LIST_DIR}/../docs/guides/typed-record-navigation.html"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides"
        COMPONENT ${_form_editor_doc_component} ${_form_editor_doc_exclude})
endforeach()
unset(_form_editor_doc_component)
unset(_form_editor_doc_exclude)

# Typed search keeps stored answers separate from the current native draft.
get_filename_component(_form_search_editor_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
foreach(_form_search_editor_component IN ITEMS Framework FrameworkSDK)
    set(_form_search_editor_exclude "")
    if(_form_search_editor_component STREQUAL "FrameworkSDK")
        set(_form_search_editor_exclude EXCLUDE_FROM_ALL)
    endif()
    install(FILES "${_form_search_editor_root}/docs/guides/native-record-search.html"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides"
        COMPONENT ${_form_search_editor_component} ${_form_search_editor_exclude})
endforeach()
get_target_property(_form_search_editor_data_definitions Umicom::data INTERFACE_COMPILE_DEFINITIONS)
if(BUILD_TESTING AND "UMICOM_HAS_SQLITE=1" IN_LIST _form_search_editor_data_definitions)
    add_executable(umicom-form-record-search-test "${_form_search_editor_root}/tests/data_history/test_form_record_search_gtk4.c")
    target_link_libraries(umicom-form-record-search-test PRIVATE Umicom::form_record_editor_gtk4)
    set_target_properties(umicom-form-record-search-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-form-record-search-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-form-record-search-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-form-record-search-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-form-record-search-test FILES
            "${_form_search_editor_root}/tests/application_experience/structured_form_fixture.h")
    endif()
    foreach(_form_search_editor_case IN ITEMS basic empty pages metadata state known-version unknown-version damaged invalid-input invalid-filter late-filter late-metadata filter-after-result late-open-filter open-stale changed-head dirty-search mode-to-metadata mode-to-typed no-next busy retire-read dispose-read retired null long-query button completion-filter acceptance-filter retained-button not-connected)
        add_test(NAME framework.structured_forms.editor.search.${_form_search_editor_case}
            COMMAND umicom-form-record-search-test ${_form_search_editor_case} "${CMAKE_CURRENT_BINARY_DIR}/form-record-search-fixtures")
        set_tests_properties(framework.structured_forms.editor.search.${_form_search_editor_case} PROPERTIES
            TIMEOUT 45 SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings"
            LABELS "framework;forms;gtk4;search;sqlite;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.structured_forms.editor.search.${_form_search_editor_case} TARGETS umicom-form-record-search-test)
        endif()
    endforeach()
endif()
unset(_form_search_editor_root)
unset(_form_search_editor_component)
unset(_form_search_editor_exclude)
unset(_form_search_editor_data_definitions)
unset(_form_search_editor_case)
