#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/windows/RenderInstaller.cmake
#
# PURPOSE:
#   Render a native NSIS setup from a complete, validated payload catalogue.
#   Application sections and exact-file uninstall actions use one inventory.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include("${CMAKE_CURRENT_LIST_DIR}/DeploymentCommon.cmake")

function(umicom_render_windows_installer)
    foreach(_required UMI_SETUP_PAYLOAD_ROOT UMI_SETUP_SCRIPT UMI_SETUP_OUTPUT_FILE
            UMI_SETUP_VERSION UMI_SETUP_APPS UMI_SETUP_LICENCE)
        if(NOT ${_required})
            message(FATAL_ERROR "Installer renderer requires ${_required}")
        endif()
    endforeach()
    if(NOT UMI_SETUP_VERSION MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+$")
        message(FATAL_ERROR "Installer version must contain three numeric components")
    endif()
    set(UMI_SETUP_SECTIONS "")
    set(UMI_SETUP_SELECTION_CHECK "")
    set(UMI_SETUP_SHORTCUT_CALLS "")
    set(UMI_SETUP_SHORTCUT_REMOVAL "")
    set(UMI_SETUP_PAYLOAD_REMOVAL "")
    set(_all_paths "")
    set(_all_titles "")
    set(_all_ids "")
    set(_inventory "")
    set(UMI_SETUP_MAX_RELATIVE 0)
    set(_index 0)
    set(_components runtime ${UMI_SETUP_APPS})
    foreach(_component IN LISTS _components)
        umicom_windows_require_identifier("${_component}")
        string(TOLOWER "${_component}" _lower_id)
        if(_lower_id IN_LIST _all_ids)
            message(FATAL_ERROR "Duplicate installer component: ${_component}")
        endif()
        list(APPEND _all_ids "${_lower_id}")
        set(COMPONENT "${_component}")
        set(_component_root "${UMI_SETUP_PAYLOAD_ROOT}/${_component}")
        if(NOT IS_DIRECTORY "${_component_root}")
            message(FATAL_ERROR "Missing installer component payload: ${_component}")
        endif()
        file(GLOB_RECURSE _files LIST_DIRECTORIES FALSE RELATIVE "${_component_root}" "${_component_root}/*")
        list(SORT _files)
        if(NOT _files)
            message(FATAL_ERROR "Empty installer component payload: ${_component}")
        endif()
        set(_directories "")
        set(_remove_files "")
        foreach(_file IN LISTS _files)
            umicom_windows_require_relative("${_file}")
            string(TOLOWER "${_file}" _control_path)
            if(_control_path STREQUAL "installation.ini" OR
               _control_path STREQUAL "uninstall-umicom.exe")
                message(FATAL_ERROR "Payload attempts to replace installer control metadata")
            endif()
            if(IS_SYMLINK "${_component_root}/${_file}")
                message(FATAL_ERROR "A symlink cannot be an owned installer file")
            endif()
            string(LENGTH "${_file}" _length)
            if(_length GREATER UMI_SETUP_MAX_RELATIVE)
                set(UMI_SETUP_MAX_RELATIVE "${_length}")
            endif()
            list(APPEND _all_paths "${_file}")
            file(SHA256 "${_component_root}/${_file}" _sha)
            string(APPEND _inventory "${_component}\t${_sha}\t${_file}\n")
            string(REPLACE "/" "\\" _windows_file "${_file}")
            umicom_windows_nsis_escape("${_windows_file}" OWNED_FILE)
            string(CONFIGURE [=[
  ${If} ${FileExists} "$INSTDIR\@OWNED_FILE@"
    ClearErrors
    Delete "$INSTDIR\@OWNED_FILE@"
    ${If} ${Errors}
      IntOp $DeletionFailures $DeletionFailures + 1
      DetailPrint "Still in use or not removable: @OWNED_FILE@"
    ${EndIf}
  ${EndIf}
]=] _remove @ONLY)
            string(APPEND _remove_files "${_remove}")
            get_filename_component(_directory "${_file}" DIRECTORY)
            while(_directory)
                list(APPEND _directories "${_directory}")
                get_filename_component(_directory "${_directory}" DIRECTORY)
            endwhile()
        endforeach()
        list(REMOVE_DUPLICATES _directories)
        # Descendants precede their parents. No recursive installed-tree delete.
        list(SORT _directories ORDER DESCENDING)
        foreach(_directory IN LISTS _directories)
            string(REPLACE "/" "\\" _directory "${_directory}")
            umicom_windows_nsis_escape("${_directory}" _directory)
            string(APPEND _remove_files "  RMDir \"\$INSTDIR\\${_directory}\"\n")
        endforeach()
        set(REMOVE_FILES "${_remove_files}")
        string(CONFIGURE [=[
  ReadINIStr $0 "$INSTDIR\installation.ini" "Applications" "@COMPONENT@"
  ${If} $0 == "1"
@REMOVE_FILES@
  ${EndIf}
]=] _remove_component @ONLY)
        string(APPEND UMI_SETUP_PAYLOAD_REMOVAL "${_remove_component}")
        if(_component STREQUAL "runtime")
            continue()
        endif()
        set(_title "${UMI_SETUP_NAME_${_component}}")
        set(_exe "${UMI_SETUP_EXE_${_component}}")
        if(NOT _title OR _title MATCHES "[<>:\"/\\\\|?*]" OR _title MATCHES "[ .]$")
            message(FATAL_ERROR "Invalid Windows shortcut/product title: ${_title}")
        endif()
        string(TOLOWER "${_title}" _title_lower)
        if(_title_lower IN_LIST _all_titles)
            message(FATAL_ERROR "Two applications would create the same shortcut: ${_title}")
        endif()
        list(APPEND _all_titles "${_title_lower}")
        umicom_windows_require_relative("${_exe}")
        if(_exe MATCHES "/" OR NOT _exe MATCHES "[.][Ee][Xx][Ee]$")
            message(FATAL_ERROR "The application executable must be an .exe basename")
        endif()
        umicom_windows_require_pe("${_component_root}/bin/${_exe}")
        umicom_windows_nsis_escape("${_title}" PRODUCT)
        umicom_windows_nsis_escape("${_exe}" EXECUTABLE)
        umicom_windows_nsis_escape("${_component_root}" PAYLOAD)
        math(EXPR _index "${_index} + 1")
        set(SECTION "UMI_APP_${_index}")
        string(CONFIGURE [=[
Section "@PRODUCT@" @SECTION@
  SectionIn 1
  ClearErrors
  WriteINIStr "$INSTDIR\installation.ini" "Applications" "@COMPONENT@" "1"
  IfErrors 0 +2
    Abort "Unable to record application installation ownership."
  SetOutPath "$INSTDIR"
  File /r "@PAYLOAD@/*"
  IfErrors 0 +2
    Abort "Unable to install @PRODUCT@."
SectionEnd
]=] _section @ONLY)
        string(APPEND UMI_SETUP_SECTIONS "${_section}\n")
        string(CONFIGURE [=[
  SectionGetFlags ${@SECTION@} $0
  IntOp $0 $0 & ${SF_SELECTED}
  IntOp $2 $2 + $0
]=] _selection @ONLY)
        string(APPEND UMI_SETUP_SELECTION_CHECK "${_selection}")
        string(CONFIGURE [=[
  Push "@COMPONENT@"
  Push "@PRODUCT@"
  Push "@EXECUTABLE@"
  Call UmiCreateApplicationShortcuts
]=] _shortcuts @ONLY)
        string(APPEND UMI_SETUP_SHORTCUT_CALLS "${_shortcuts}")
        string(CONFIGURE [=[
  ReadINIStr $0 "$INSTDIR\installation.ini" "Shortcuts" "desktop-@COMPONENT@"
  ${If} $0 == "1"
    ${If} ${FileExists} "$DESKTOP\@PRODUCT@.lnk"
      ClearErrors
      Delete "$DESKTOP\@PRODUCT@.lnk"
      ${If} ${Errors}
        IntOp $DeletionFailures $DeletionFailures + 1
      ${EndIf}
    ${EndIf}
  ${EndIf}
  ReadINIStr $0 "$INSTDIR\installation.ini" "Shortcuts" "menu-@COMPONENT@"
  ${If} $0 == "1"
    ${If} ${FileExists} "$SMPROGRAMS\Umicom Applications\@PRODUCT@.lnk"
      ClearErrors
      Delete "$SMPROGRAMS\Umicom Applications\@PRODUCT@.lnk"
      ${If} ${Errors}
        IntOp $DeletionFailures $DeletionFailures + 1
      ${EndIf}
    ${EndIf}
  ${EndIf}
]=] _shortcuts_remove @ONLY)
        string(APPEND UMI_SETUP_SHORTCUT_REMOVAL "${_shortcuts_remove}")
    endforeach()
    umicom_windows_validate_unique_paths(${_all_paths})
    string(SHA256 UMI_SETUP_BUNDLE_ID "${_inventory}")
    umicom_windows_nsis_escape("${UMI_SETUP_OUTPUT_FILE}" UMI_SETUP_OUTPUT_ESCAPED)
    umicom_windows_nsis_escape("${UMI_SETUP_PAYLOAD_ROOT}/runtime" UMI_SETUP_RUNTIME_ESCAPED)
    umicom_windows_nsis_escape("${UMI_SETUP_LICENCE}" UMI_SETUP_LICENCE_ESCAPED)
    set(UMI_SETUP_ICON_CODE "")
    if(UMI_SETUP_ICON AND EXISTS "${UMI_SETUP_ICON}")
        umicom_windows_nsis_escape("${UMI_SETUP_ICON}" _icon)
        set(UMI_SETUP_ICON_CODE
            "!define MUI_ICON \"${_icon}\"\n!define MUI_UNICON \"${_icon}\"")
    endif()
    get_filename_component(_output_dir "${UMI_SETUP_SCRIPT}" DIRECTORY)
    file(MAKE_DIRECTORY "${_output_dir}")
    file(WRITE "${_output_dir}/payload-sha256.tsv" "component\tsha256\trelative_path\n${_inventory}")
    configure_file("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/UmicomSetup.nsi.in"
        "${UMI_SETUP_SCRIPT}" @ONLY NEWLINE_STYLE LF)
endfunction()
