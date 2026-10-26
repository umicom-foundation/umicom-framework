# Umicom Framework
# File: cmake/UmicomWorkspaceRename.cmake
# PURPOSE: Register no-replacement workspace operations and open-document guards with their owning libraries.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
if(NOT TARGET umicom_platform)
    return()
endif()
include_guard(GLOBAL)
include(GNUInstallDirs)
get_filename_component(_rename_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
target_sources(umicom_platform PRIVATE "${_rename_root}/src/platform/workspace_rename.c")
install(FILES "${_rename_root}/include/umicom/platform/workspace_rename.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/platform" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_platform include/umicom/platform/workspace_rename.h)
endif()
if(TARGET umicom_document)
    target_sources(umicom_document PRIVATE "${_rename_root}/src/document/path_usage.c")
    install(FILES "${_rename_root}/include/umicom/document/path_usage.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/document" COMPONENT Framework)
    if(COMMAND umicom_release_inventory_own_headers)
        umicom_release_inventory_own_headers(umicom_document include/umicom/document/path_usage.h)
    endif()
endif()
install(FILES "${_rename_root}/docs/guides/renaming-project-files.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides" COMPONENT Framework)
if(BUILD_TESTING)
    set(_rename_preview_cases basic nested backslash same empty-source empty-name parent-source
        parent-name absolute-source absolute-name new-folder new-folder-backslash nested-new-folder
        metadata-source metadata-name device
        colon trailing-dot trailing-space stale untrusted closed project-root project-parent arguments capacity)
    set(_rename_filesystem_cases file nested directory existing-file existing-directory missing-source
        missing-parent unicode unchanged arguments stale untrusted)
    set(_rename_documents_cases source destination descendant sibling inactive dirty untitled closed arguments normalised)
    set(_rename_race_cases race)
    set(_rename_links_cases source parent)
    foreach(_rename_group IN ITEMS preview filesystem race links documents)
        if(_rename_group STREQUAL "documents")
            if(NOT TARGET umicom_document)
                continue()
            endif()
            set(_rename_source "tests/document/test_path_usage.c")
            set(_rename_library Umicom::document)
        else()
            set(_rename_source "tests/workspace_rename/test_${_rename_group}.c")
            set(_rename_library Umicom::platform)
        endif()
        set(_rename_target "umicom-workspace-rename-${_rename_group}-test")
        add_executable(${_rename_target} "${_rename_root}/${_rename_source}")
        target_link_libraries(${_rename_target} PRIVATE ${_rename_library})
        set_target_properties(${_rename_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_rename_target})
        umicom_apply_sanitizers(${_rename_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_rename_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_rename_target} FILES
                "${_rename_root}/tests/workspace_rename/fixture.h"
                "${_rename_root}/src/platform/workspace_rename_windows.inc"
                "${_rename_root}/src/platform/workspace_rename_linux.inc"
                "${_rename_root}/src/platform/workspace_name_internal.h")
        endif()
        foreach(_rename_case IN LISTS _rename_${_rename_group}_cases)
            set(_rename_test "framework.workspace.rename.${_rename_group}.${_rename_case}")
            add_test(NAME ${_rename_test} COMMAND ${_rename_target} ${_rename_case})
            file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/workspace-rename/${_rename_group}/${_rename_case}")
            set_tests_properties(${_rename_test} PROPERTIES TIMEOUT 45 SKIP_RETURN_CODE 77
                WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qualification/workspace-rename/${_rename_group}/${_rename_case}"
                LABELS "framework;workspace;rename;filesystem;ownership;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(${_rename_test} TARGETS ${_rename_target})
            endif()
        endforeach()
    endforeach()
endif()
unset(_rename_root)
unset(_rename_group)
unset(_rename_case)
unset(_rename_test)
unset(_rename_target)
unset(_rename_source)
unset(_rename_library)
unset(_rename_preview_cases)
unset(_rename_filesystem_cases)
unset(_rename_documents_cases)
unset(_rename_race_cases)
unset(_rename_links_cases)
