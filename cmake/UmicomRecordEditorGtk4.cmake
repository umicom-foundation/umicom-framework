# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# GTK composition depends on the same portable data library exported to native
# and headless consumers. An absent GUI capability does not disable data tests.
if(NOT TARGET Umicom::ui_gtk4 OR NOT TARGET Umicom::data)
    return()
endif()
include_guard(GLOBAL)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
include(GNUInstallDirs)
get_filename_component(_record_editor_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
add_library(umicom_record_editor_gtk4 STATIC "${_record_editor_root}/adapters/gtk4/record_editor_gtk4.c")
add_library(Umicom::record_editor_gtk4 ALIAS umicom_record_editor_gtk4)
set_target_properties(umicom_record_editor_gtk4 PROPERTIES EXPORT_NAME record_editor_gtk4
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom_record_editor_gtk4 PUBLIC Umicom::data Umicom::ui_gtk4)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_record_editor_gtk4)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_record_editor_gtk4)
endif()
install(TARGETS umicom_record_editor_gtk4 EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
umicom_register_sdk_libraries(umicom_record_editor_gtk4)
install(FILES "${_record_editor_root}/include/umicom/ui/gtk4/record_editor.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_record_editor_gtk4 include/umicom/ui/gtk4/record_editor.h)
endif()
foreach(_record_editor_component IN ITEMS Framework FrameworkSDK)
    if(_record_editor_component STREQUAL "FrameworkSDK")
        set(_record_editor_exclude EXCLUDE_FROM_ALL)
    else()
        set(_record_editor_exclude "")
    endif()
    install(FILES "${_record_editor_root}/docs/guides/native-record-editor.html"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides"
        COMPONENT ${_record_editor_component} ${_record_editor_exclude})
    install(DIRECTORY "${_record_editor_root}/examples/record_editor/"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/examples/record_editor"
        COMPONENT ${_record_editor_component} ${_record_editor_exclude})
endforeach()
if(BUILD_TESTING)
    add_executable(umicom-record-editor-test "${_record_editor_root}/tests/data_history/test_record_editor_gtk4.c")
    target_link_libraries(umicom-record-editor-test PRIVATE Umicom::record_editor_gtk4)
    set_target_properties(umicom-record-editor-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-record-editor-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-record-editor-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-record-editor-test)
    endif()
    set(_record_editor_cases options relative-path retired-actions)
    if(SQLite3_FOUND)
        list(APPEND _record_editor_cases roundtrip reopen stale later-edit oversize invalid-attribution
            busy dirty-open binary invalid-utf8 acceptance-close completion-close acceptance-edit completion-edit unroot close-guard
            retire-read retire-save compare-preserves compare-adopt compare-stale compare-discard compare-close
            history-pages history-stale history-content history-invalid history-close metadata-utf8)
    endif()
    foreach(_record_editor_case IN LISTS _record_editor_cases)
        add_test(NAME framework.data.editor.gtk.${_record_editor_case}
            COMMAND umicom-record-editor-test ${_record_editor_case}
                "${CMAKE_CURRENT_BINARY_DIR}/record-editor-fixtures/${_record_editor_case}")
        set_tests_properties(framework.data.editor.gtk.${_record_editor_case} PROPERTIES
            TIMEOUT 45 SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings"
            LABELS "framework;data;record-editor;gtk4;lifetime;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.data.editor.gtk.${_record_editor_case} TARGETS umicom-record-editor-test)
        endif()
    endforeach()
endif()
unset(_record_editor_root)
unset(_record_editor_component)
unset(_record_editor_exclude)
unset(_record_editor_cases)
unset(_record_editor_case)
