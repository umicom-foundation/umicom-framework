# Umicom Framework
# File: cmake/UmicomEditorPreferenceStorage.cmake
# PURPOSE: Integrate bounded preference records and local storage with the owning SDK targets.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
if(NOT TARGET umicom_editor)
    return()
endif()
include_guard(GLOBAL)
include(GNUInstallDirs)
get_filename_component(_preference_storage_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
target_sources(umicom_editor PRIVATE "${_preference_storage_root}/src/editor/indentation_record.c")
install(FILES "${_preference_storage_root}/include/umicom/editor/indentation_record.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/editor" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_editor include/umicom/editor/indentation_record.h)
endif()
if(TARGET umicom_document)
    target_sources(umicom_document PRIVATE "${_preference_storage_root}/src/document/indentation_storage.c")
    install(FILES "${_preference_storage_root}/include/umicom/document/indentation_storage.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/document" COMPONENT Framework)
    if(COMMAND umicom_release_inventory_own_headers)
        umicom_release_inventory_own_headers(umicom_document include/umicom/document/indentation_storage.h)
    endif()
endif()
install(FILES "${_preference_storage_root}/docs/guides/editor-preference-storage.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides" COMPONENT Framework)
if(BUILD_TESTING)
    set(_preference_storage_editor_cases spaces tabs one sixteen crlf no-newline version order extra duplicate
        zero wide boolean nul empty large capacity arguments invalid-encode bare-cr)
    set(_preference_storage_document_cases round-trip replace invalid-save malformed oversized missing
        directory unicode path path-short path-relative arguments)
    foreach(_preference_storage_group IN ITEMS editor document)
        if(NOT TARGET umicom_${_preference_storage_group})
            continue()
        endif()
        set(_preference_storage_target "umicom-preference-storage-${_preference_storage_group}-test")
        if(_preference_storage_group STREQUAL "editor")
            set(_preference_storage_source tests/editor/test_indentation_record.c)
        else()
            set(_preference_storage_source tests/document/test_indentation_storage.c)
        endif()
        add_executable(${_preference_storage_target} "${_preference_storage_root}/${_preference_storage_source}")
        target_link_libraries(${_preference_storage_target} PRIVATE Umicom::${_preference_storage_group})
        set_target_properties(${_preference_storage_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(${_preference_storage_target})
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(${_preference_storage_target})
        endif()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_preference_storage_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_preference_storage_target} FILES
                "${_preference_storage_root}/tests/build_log/fixture.h"
                "${_preference_storage_root}/tests/support/symbolic_link.h")
        endif()
        foreach(_preference_storage_case IN LISTS _preference_storage_${_preference_storage_group}_cases)
            set(_preference_storage_test "framework.editor.preference_storage.${_preference_storage_group}.${_preference_storage_case}")
            add_test(NAME ${_preference_storage_test} COMMAND ${_preference_storage_target} ${_preference_storage_case})
            # Case directory names are prefixed because NUL is a Windows device name.
            # The previous path is retained as a comment for portability review.
            # file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/preference-storage/${_preference_storage_group}/${_preference_storage_case}")
            file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/preference-storage/${_preference_storage_group}/case-${_preference_storage_case}")
            set_tests_properties(${_preference_storage_test} PROPERTIES TIMEOUT 30
            # Case directory names are prefixed because NUL is a Windows device name.
            # The previous path is retained as a comment for portability review.
            # WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/preference-storage/${_preference_storage_group}/${_preference_storage_case}"
                WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/preference-storage/${_preference_storage_group}/case-${_preference_storage_case}"
                LABELS "framework;editor;preferences;storage;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(${_preference_storage_test} TARGETS ${_preference_storage_target})
            endif()
        endforeach()
    endforeach()
endif()
unset(_preference_storage_root)
unset(_preference_storage_editor_cases)
unset(_preference_storage_document_cases)
unset(_preference_storage_target)
unset(_preference_storage_source)
unset(_preference_storage_group)
unset(_preference_storage_case)
unset(_preference_storage_test)
