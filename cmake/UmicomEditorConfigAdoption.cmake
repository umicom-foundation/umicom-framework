# Umicom Framework
# File: cmake/UmicomEditorConfigAdoption.cmake
# PURPOSE: Export reviewed configuration adoption and register isolated owner-run coverage.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
if(NOT TARGET umicom_document)
    return()
endif()
include_guard(GLOBAL)
include(GNUInstallDirs)
get_filename_component(_adoption_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
target_sources(umicom_document PRIVATE
    "${_adoption_root}/src/document/editor_config_evidence.c"
    "${_adoption_root}/src/document/editor_config_adoption.c")
foreach(_adoption_header IN ITEMS editor_config_evidence.h editor_config_adoption.h)
    install(FILES "${_adoption_root}/include/umicom/document/${_adoption_header}"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/document" COMPONENT Framework)
    if(COMMAND umicom_release_inventory_own_headers)
        umicom_release_inventory_own_headers(umicom_document "include/umicom/document/${_adoption_header}")
    endif()
endforeach()
if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE "${_adoption_root}/adapters/gtk4/editor_config_adoption_gtk4.c")
    install(FILES "${_adoption_root}/include/umicom/ui/gtk4/editor_config_adoption.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
    if(COMMAND umicom_release_inventory_own_headers)
        umicom_release_inventory_own_headers(umicom_ui_gtk4 include/umicom/ui/gtk4/editor_config_adoption.h)
    endif()
endif()
install(FILES "${_adoption_root}/docs/guides/adopt-project-indentation.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides" COMPONENT Framework)
if(BUILD_TESTING)
    set(_adoption_evidence_cases same scope copy-ownership changed-value changed-comment closer-added removed different-target different-root normalized-root same-root-stop arguments)
    set(_adoption_plan_cases proposal apply style-only tab-size unrepresentable large-width invalid-style invalid-size invalid-tab-width missing unset nonindent unknown invalid-other stale-file closer-added removed comment-change stale-default stale-override save-as closed other-document draft readonly repeated ownership arguments source-metadata untitled foreign-owner)
    set(_adoption_native_cases inspect apply invalid unsupported cancel-inspect cancel-apply change-config add-config stale-default closed-target inactive retire-inspect retire-apply close-inspect close-apply repeat busy not-inspected arguments refresh-retire applied-message-close)
    foreach(_adoption_group IN ITEMS evidence plan native)
        set(_adoption_owner document)
        set(_adoption_source tests/document/test_editor_config_evidence.c)
        if(_adoption_group STREQUAL "plan")
            set(_adoption_source tests/document/test_editor_config_adoption.c)
        elseif(_adoption_group STREQUAL "native")
            set(_adoption_owner ui_gtk4)
            set(_adoption_source tests/document/test_editor_config_adoption_gtk4.c)
        endif()
        if(NOT TARGET umicom_${_adoption_owner})
            continue()
        endif()
        set(_adoption_target "umicom-editor-config-adoption-${_adoption_group}-test")
        add_executable(${_adoption_target} "${_adoption_root}/${_adoption_source}")
        target_link_libraries(${_adoption_target} PRIVATE Umicom::${_adoption_owner})
        set_target_properties(${_adoption_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(${_adoption_target})
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(${_adoption_target})
        endif()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_adoption_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_adoption_target} FILES
                "${_adoption_root}/tests/document/editor_config_adoption_fixture.h"
                "${_adoption_root}/tests/build_log/fixture.h")
        endif()
        foreach(_adoption_case IN LISTS _adoption_${_adoption_group}_cases)
            set(_adoption_test "framework.editor.config.adoption.${_adoption_group}.${_adoption_case}")
            file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/config-adoption/${_adoption_group}/${_adoption_case}")
            add_test(NAME ${_adoption_test} COMMAND ${_adoption_target} ${_adoption_case})
            set_tests_properties(${_adoption_test} PROPERTIES TIMEOUT 60
                WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/config-adoption/${_adoption_group}/${_adoption_case}"
                LABELS "framework;document;editor;configuration;regression")
            if(_adoption_owner STREQUAL "ui_gtk4")
                set_tests_properties(${_adoption_test} PROPERTIES SKIP_RETURN_CODE 77
                    ENVIRONMENT "G_DEBUG=fatal-warnings" LABELS "framework;document;editor;configuration;gtk4;regression")
            endif()
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(${_adoption_test} TARGETS ${_adoption_target})
            endif()
        endforeach()
    endforeach()
endif()
unset(_adoption_root)
unset(_adoption_header)
unset(_adoption_group)
unset(_adoption_owner)
unset(_adoption_source)
unset(_adoption_target)
unset(_adoption_test)
unset(_adoption_case)
unset(_adoption_evidence_cases)
unset(_adoption_plan_cases)
unset(_adoption_native_cases)

# Compile the native public header on its own before its integration test.
# This catches missing GTK declarations even when the test fixture happens
# to include GTK first. Reuse the Framework header-consumer mechanism.
if(BUILD_TESTING AND TARGET umicom-editor-config-adoption-native-test)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomPublicHeaderCompileChecks.cmake")
    umicom_add_public_header_compile_check(umicom-editor-config-adoption-header-check
        umicom_ui_gtk4 "${CMAKE_CURRENT_LIST_DIR}/../include"
        "umicom/ui/gtk4/editor_config_adoption.h")
    add_dependencies(umicom-editor-config-adoption-native-test umicom-editor-config-adoption-header-check)
endif()
