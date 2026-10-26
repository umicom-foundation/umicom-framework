# Umicom Framework
# File: cmake/UmicomEditorConfig.cmake
# PURPOSE: Register shared project editor configuration, SDK ownership and regression targets.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
if(NOT TARGET umicom_editor)
    return()
endif()
include_guard(GLOBAL)
include(GNUInstallDirs)
get_filename_component(_config_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
target_sources(umicom_editor PRIVATE
    "${_config_root}/src/editor/editor_config.c"
    "${_config_root}/src/editor/editor_config_text.c"
    "${_config_root}/src/editor/editor_config_pattern.c"
    "${_config_root}/src/editor/editor_config_result.c"
    "${_config_root}/src/editor/editor_config_settings.c")
foreach(_config_header IN ITEMS editor_config.h editor_config_pattern.h editor_config_result.h editor_config_settings.h)
    install(FILES "${_config_root}/include/umicom/editor/${_config_header}"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/editor" COMPONENT Framework)
    if(COMMAND umicom_release_inventory_own_headers)
        umicom_release_inventory_own_headers(umicom_editor "include/umicom/editor/${_config_header}")
    endif()
endforeach()
if(TARGET umicom_document)
    target_sources(umicom_document PRIVATE "${_config_root}/src/document/editor_config_inspection.c")
    install(FILES "${_config_root}/include/umicom/document/editor_config_inspection.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/document" COMPONENT Framework)
    if(COMMAND umicom_release_inventory_own_headers)
        umicom_release_inventory_own_headers(umicom_document include/umicom/document/editor_config_inspection.h)
    endif()
endif()
if(TARGET umicom_ui_gtk4 AND TARGET umicom_document)
    target_sources(umicom_ui_gtk4 PRIVATE "${_config_root}/adapters/gtk4/editor_config_inspection_gtk4.c")
    install(FILES "${_config_root}/include/umicom/ui/gtk4/editor_config_inspection.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
    if(COMMAND umicom_release_inventory_own_headers)
        umicom_release_inventory_own_headers(umicom_ui_gtk4 include/umicom/ui/gtk4/editor_config_inspection.h)
    endif()
endif()
install(FILES "${_config_root}/docs/guides/project-editor-config.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides" COMPONENT Framework)
if(BUILD_TESTING)
    set(_config_parse_cases empty comments root root-last section-root trimming inline-comment empty-value equals-value unicode bom crlf bare-cr bare-cr-middle no-assignment empty-key empty-section section-trailing unknown-key preserve-value-case arguments nul utf8 size maximum-key long-key maximum-value long-value section-limit pair-limit ownership)
    set(_config_pattern_cases basename case single-star double-star zero-directories deep-directories leading-slash root-only unicode-question unicode-class negative-class negative-refusal literal-class literal-hyphen alternatives nested-alternatives numeric negative-number numeric-miss literal-brace escaped-star escaped-question class-metacharacters recursive-root recursive-many directory-pattern traversal empty-component backslash-path absolute-path malformed-class dangling-escape empty-pattern empty-path wide-numeric-range leading-zero-number positive-number drive-path arguments pattern-limit path-limit expansion-limit malformed-utf8 work-budget)
    set(_config_result_cases section-order same-section unmatched preamble-ignored unset unset-then-restore case-key unknown-property basename-nested brace-pattern empty-value empty-config layer-precedence inner-root layer-unset malformed-pattern invalid-empty-path arguments layer-capacity)
    set(_config_settings_cases default spaces tabs tab-size tab-size-default equal-widths distinct-widths large-width invalid-number zero-number negative-number overflow-number unknown-style mixed-case unset-default format-values invalid-format arguments)
    set(_config_inspection_cases inherit root-stop boundary missing missing-directory unicode malformed oversized directory traversal cancelled arguments metadata unchanged case-selection)
    set(_config_gtk4_cases inspect empty malformed cancel retired changed-input repeat readonly close-report arguments)
    foreach(_config_group IN ITEMS parse pattern result settings inspection gtk4)
        set(_config_owner editor)
        set(_config_source "tests/editor/test_editor_config_${_config_group}.c")
        if(_config_group STREQUAL "inspection")
            set(_config_owner document)
            set(_config_source tests/document/test_editor_config_inspection.c)
        elseif(_config_group STREQUAL "gtk4")
            set(_config_owner ui_gtk4)
            set(_config_source tests/document/test_editor_config_inspection_gtk4.c)
        endif()
        if(NOT TARGET umicom_${_config_owner})
            continue()
        endif()
        set(_config_target "umicom-editor-config-${_config_group}-test")
        add_executable(${_config_target} "${_config_root}/${_config_source}")
        target_link_libraries(${_config_target} PRIVATE Umicom::${_config_owner})
        set_target_properties(${_config_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(${_config_target})
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(${_config_target})
        endif()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_config_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_config_target} FILES
                "${_config_root}/tests/editor/editor_config_fixture.h"
                "${_config_root}/src/editor/editor_config_internal.h"
                "${_config_root}/tests/build_log/fixture.h")
        endif()
        foreach(_config_case IN LISTS _config_${_config_group}_cases)
            set(_config_test "framework.editor.config.${_config_group}.${_config_case}")
            add_test(NAME ${_config_test} COMMAND ${_config_target} ${_config_case})
            # Case directory names are prefixed because NUL is a Windows device name.
            # The previous path is retained as a comment for portability review.
            # file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/editor-config/${_config_group}/${_config_case}")
            file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/editor-config/${_config_group}/case-${_config_case}")
            set_tests_properties(${_config_test} PROPERTIES TIMEOUT 60
            # Case directory names are prefixed because NUL is a Windows device name.
            # The previous path is retained as a comment for portability review.
            # WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/editor-config/${_config_group}/${_config_case}"
                WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/editor-config/${_config_group}/case-${_config_case}"
                LABELS "framework;editor;configuration;regression")
            if(_config_group STREQUAL "gtk4")
                set_tests_properties(${_config_test} PROPERTIES SKIP_RETURN_CODE 77
                    ENVIRONMENT "G_DEBUG=fatal-warnings" LABELS "framework;editor;configuration;gtk4;regression")
            endif()
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(${_config_test} TARGETS ${_config_target})
            endif()
        endforeach()
    endforeach()
endif()
unset(_config_root)
unset(_config_header)
unset(_config_group)
unset(_config_owner)
unset(_config_source)
unset(_config_target)
unset(_config_case)
unset(_config_test)
unset(_config_parse_cases)
unset(_config_pattern_cases)
unset(_config_result_cases)
unset(_config_settings_cases)
unset(_config_inspection_cases)
unset(_config_gtk4_cases)
