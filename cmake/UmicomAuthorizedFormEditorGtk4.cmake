# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Applications compose one retained record host with the shared typed editor.
if(NOT TARGET Umicom::record_host OR NOT TARGET Umicom::structured_form_gtk4)
    return()
endif()
include_guard(GLOBAL)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
include(GNUInstallDirs)
get_filename_component(_authorized_form_editor_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
add_library(umicom_authorized_form_editor_gtk4 STATIC
    "${_authorized_form_editor_root}/adapters/gtk4/authorized_form_editor_gtk4.c")
add_library(Umicom::authorized_form_editor_gtk4 ALIAS umicom_authorized_form_editor_gtk4)
target_link_libraries(umicom_authorized_form_editor_gtk4 PUBLIC
    Umicom::record_host Umicom::structured_form_gtk4)
set_target_properties(umicom_authorized_form_editor_gtk4 PROPERTIES
    EXPORT_NAME authorized_form_editor_gtk4 C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_authorized_form_editor_gtk4)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_authorized_form_editor_gtk4)
endif()
install(TARGETS umicom_authorized_form_editor_gtk4 EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
umicom_register_sdk_libraries(umicom_authorized_form_editor_gtk4)
install(FILES "${_authorized_form_editor_root}/include/umicom/ui/gtk4/authorized_form_editor.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_authorized_form_editor_gtk4
        include/umicom/ui/gtk4/authorized_form_editor.h)
endif()
foreach(_authorized_form_editor_component IN ITEMS Framework FrameworkSDK)
    set(_authorized_form_editor_exclude "")
    if(_authorized_form_editor_component STREQUAL "FrameworkSDK")
        set(_authorized_form_editor_exclude EXCLUDE_FROM_ALL)
    endif()
    install(FILES "${_authorized_form_editor_root}/docs/guides/authorized-native-forms.html"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides"
        COMPONENT ${_authorized_form_editor_component} ${_authorized_form_editor_exclude})
endforeach()
if(BUILD_TESTING)
    set(_authorized_form_editor_lifecycle_cases initial begin invalid-id long-id dirty-begin dirty-open
        discard-new old-controls discard-saved retired dispose unroot retained-host buttons-new
        buttons-open buttons-discard invalid-session invalid-purpose null-schema null)
    set(_authorized_form_editor_persistence_cases open missing open-denied create scope clean-save
        invalid-value amend buttons-save submit-missing invalid-reason unclosed-reason disabled
        revoked expired policy-denied host-retired stale-head)
    set(_authorized_form_editor_ownership_cases disposed-load late-open type-on-apply retire-on-apply
        dispose-on-apply nested-on-apply late-input late-save-retry late-applied discard-after-late-save
        retired-save disposed-save busy nested-on-complete retire-on-complete)
    set(_authorized_form_editor_backends memory)
    if(SQLite3_FOUND)
        list(APPEND _authorized_form_editor_backends sqlite)
    endif()
    foreach(_authorized_form_editor_group IN ITEMS lifecycle persistence ownership)
        set(_authorized_form_editor_target "umicom-authorized-form-editor-${_authorized_form_editor_group}-test")
        add_executable(${_authorized_form_editor_target}
            "${_authorized_form_editor_root}/tests/data_history/test_authorized_form_editor_${_authorized_form_editor_group}.c")
        target_link_libraries(${_authorized_form_editor_target} PRIVATE Umicom::authorized_form_editor_gtk4)
        set_target_properties(${_authorized_form_editor_target} PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(${_authorized_form_editor_target})
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(${_authorized_form_editor_target})
        endif()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_authorized_form_editor_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_authorized_form_editor_target} FILES
                "${_authorized_form_editor_root}/tests/data_history/authorized_form_editor_fixture.h"
                "${_authorized_form_editor_root}/tests/data_history/record_host_fixture.h"
                "${_authorized_form_editor_root}/tests/application_experience/structured_form_fixture.h")
        endif()
        foreach(_authorized_form_editor_backend IN LISTS _authorized_form_editor_backends)
            foreach(_authorized_form_editor_case IN LISTS _authorized_form_editor_${_authorized_form_editor_group}_cases)
                set(_authorized_form_editor_test "framework.authorized_form_editor.${_authorized_form_editor_group}.${_authorized_form_editor_backend}.${_authorized_form_editor_case}")
                add_test(NAME ${_authorized_form_editor_test} COMMAND ${_authorized_form_editor_target}
                    ${_authorized_form_editor_backend} ${_authorized_form_editor_case})
                set_tests_properties(${_authorized_form_editor_test} PROPERTIES TIMEOUT 45 SKIP_RETURN_CODE 77
                    ENVIRONMENT "G_DEBUG=fatal-warnings"
                    LABELS "framework;forms;gtk4;security;${_authorized_form_editor_backend};regression")
                if(COMMAND umicom_test_impact_owners)
                    umicom_test_impact_owners(${_authorized_form_editor_test} TARGETS ${_authorized_form_editor_target})
                endif()
            endforeach()
        endforeach()
    endforeach()
    add_executable(umicom-authorized-form-native-example
        "${_authorized_form_editor_root}/examples/structured_forms/authorized_native.c")
    target_link_libraries(umicom-authorized-form-native-example PRIVATE Umicom::authorized_form_editor_gtk4)
    set_target_properties(umicom-authorized-form-native-example PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-authorized-form-native-example)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-authorized-form-native-example)
    endif()
endif()
unset(_authorized_form_editor_root)
unset(_authorized_form_editor_component)
unset(_authorized_form_editor_exclude)
unset(_authorized_form_editor_group)
unset(_authorized_form_editor_target)
unset(_authorized_form_editor_test)
unset(_authorized_form_editor_case)
unset(_authorized_form_editor_backend)
unset(_authorized_form_editor_backends)
unset(_authorized_form_editor_lifecycle_cases)
unset(_authorized_form_editor_persistence_cases)
unset(_authorized_form_editor_ownership_cases)
