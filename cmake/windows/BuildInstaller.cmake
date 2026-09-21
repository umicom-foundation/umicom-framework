#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/windows/BuildInstaller.cmake
#
# PURPOSE:
#   Build one native setup executable and a portable package from the exact
#   executable/runtime reports produced by the configured application targets.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
include("${CMAKE_CURRENT_LIST_DIR}/DeploymentCommon.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/RenderInstaller.cmake")
if(NOT UMI_PACKAGE_CONFIG OR NOT EXISTS "${UMI_PACKAGE_CONFIG}")
    message(FATAL_ERROR "A generated UMI_PACKAGE_CONFIG is required")
endif()
include("${UMI_PACKAGE_CONFIG}")
if(NOT UMI_PACKAGE_TARGETS OR NOT UMI_PACKAGE_OUTPUT)
    message(FATAL_ERROR "No built GUI applications were registered for this installer")
endif()
set(_nsis "${UMI_PACKAGE_NSIS}")
if(NOT _nsis)
    find_program(_nsis_found NAMES makensis makensis.exe
        HINTS "$ENV{ProgramFiles}/NSIS" "$ENV{ProgramW6432}/NSIS"
            "C:/Program Files (x86)/NSIS")
    set(_nsis "${_nsis_found}")
endif()
if(NOT _nsis OR NOT EXISTS "${_nsis}")
    message(FATAL_ERROR
        "NSIS is required on the build machine to create Umicom-Setup.exe. "
        "Install NSIS 3 and reconfigure with -DUMICOM_NSIS_EXECUTABLE=<path-to-makensis.exe>. "
        "End users do not need NSIS, GTK or MSYS2.")
endif()
file(MAKE_DIRECTORY "${UMI_PACKAGE_OUTPUT}")
file(LOCK "${UMI_PACKAGE_OUTPUT}/.umicom-package.lock" GUARD PROCESS TIMEOUT 300)
set(_work "${UMI_PACKAGE_OUTPUT}/work")
umicom_windows_reset_owned_directory("${_work}")
set(UMI_SETUP_PAYLOAD_ROOT "${_work}/payload")
file(MAKE_DIRECTORY "${UMI_SETUP_PAYLOAD_ROOT}/runtime")
set(_runtime_keys "")
set(_runtime_records "")
set(_runtime_hashes "")
set(UMI_SETUP_APPS "${UMI_PACKAGE_TARGETS}")
set(_all_package_targets ${UMI_PACKAGE_TARGETS} ${UMI_PACKAGE_HELPERS})
foreach(_target IN LISTS _all_package_targets)
    umicom_windows_require_identifier("${_target}")
    set(_config "${UMI_PACKAGE_CONFIG_${_target}}")
    if(NOT EXISTS "${_config}")
        message(FATAL_ERROR "Missing deployment configuration for ${_target}")
    endif()
    include("${_config}")
    set(_report "${UMI_DEPLOY_WORK}/${_target}.runtime.cmake")
    if(NOT EXISTS "${_report}")
        message(FATAL_ERROR "Build ${_target}-windows-runtime before creating its installer")
    endif()
    include("${_report}")
    if(NOT UMI_REPORT_TARGET STREQUAL _target)
        message(FATAL_ERROR "The runtime report belongs to another target")
    endif()
    umicom_windows_require_pe("${UMI_REPORT_EXECUTABLE}")
    file(SHA256 "${UMI_REPORT_EXECUTABLE}" _executable_sha)
    if(NOT _executable_sha STREQUAL UMI_REPORT_EXECUTABLE_SHA256)
        message(FATAL_ERROR "The executable changed after runtime staging: ${_target}")
    endif()
    get_filename_component(_exe "${UMI_REPORT_EXECUTABLE}" NAME)
    if(_target IN_LIST UMI_PACKAGE_TARGETS)
        set(UMI_SETUP_NAME_${_target} "${UMI_REPORT_PRODUCT}")
        set(UMI_SETUP_EXE_${_target} "${_exe}")
        umicom_windows_copy_file("${UMI_REPORT_EXECUTABLE}"
            "${UMI_SETUP_PAYLOAD_ROOT}/${_target}/bin/${_exe}")
    else()
        list(APPEND UMI_REPORT_FILES "bin/${_exe}|${UMI_REPORT_EXECUTABLE}|${_executable_sha}")
    endif()
    foreach(_record IN LISTS UMI_REPORT_FILES)
        string(REPLACE "|" ";" _fields "${_record}")
        list(LENGTH _fields _count)
        if(NOT _count EQUAL 3)
            message(FATAL_ERROR "Malformed runtime ownership record for ${_target}")
        endif()
        list(GET _fields 0 _relative)
        list(GET _fields 1 _source)
        list(GET _fields 2 _expected_hash)
        umicom_windows_require_relative("${_relative}")
        if(NOT EXISTS "${_source}" OR IS_DIRECTORY "${_source}" OR IS_SYMLINK "${_source}")
            message(FATAL_ERROR "A declared runtime file is missing or not regular: ${_source}")
        endif()
        string(TOLOWER "${_relative}" _key)
        file(SHA256 "${_source}" _hash)
        if(NOT _hash STREQUAL _expected_hash)
            message(FATAL_ERROR "The staged runtime changed after validation: ${_source}")
        endif()
        list(FIND _runtime_keys "${_key}" _previous)
        if(_previous GREATER_EQUAL 0)
            list(GET _runtime_hashes "${_previous}" _previous_hash)
            list(GET _runtime_records "${_previous}" _previous_relative)
            if(NOT _hash STREQUAL _previous_hash OR NOT _relative STREQUAL _previous_relative)
                message(FATAL_ERROR "Application runtimes conflict at ${_relative}")
            endif()
        else()
            list(APPEND _runtime_keys "${_key}")
            list(APPEND _runtime_hashes "${_hash}")
            list(APPEND _runtime_records "${_relative}")
            umicom_windows_copy_file("${_source}" "${UMI_SETUP_PAYLOAD_ROOT}/runtime/${_relative}")
        endif()
    endforeach()
endforeach()
set(_framework "${UMI_PACKAGE_FRAMEWORK_SUPPORT}")
if(NOT _framework)
    message(FATAL_ERROR "The installer configuration is missing its Framework support root")
endif()
umicom_windows_copy_file("${_framework}/LICENSE"
    "${UMI_SETUP_PAYLOAD_ROOT}/runtime/share/umicom/licences/Umicom-LICENSE.txt")
umicom_windows_copy_file("${_framework}/docs/windows-installation.html"
    "${UMI_SETUP_PAYLOAD_ROOT}/runtime/share/umicom/docs/windows-installation.html")
file(READ "${_framework}/LICENSE" _licence)
string(APPEND _licence
    "\nTHIRD-PARTY COMPONENTS\n\n"
    "The MIT licence above applies to Umicom code, not to every bundled library.\n"
    "Bundled third-party notices are installed under share/licenses.\n"
    "GTK, GLib and other dependencies retain their own licences.\n"
    "The distributor must meet the corresponding source and redistribution\n"
    "requirements for the exact library versions included in a release.\n")
set(UMI_SETUP_LICENCE "${_work}/installer-licence.txt")
file(WRITE "${UMI_SETUP_LICENCE}" "${_licence}")
set(UMI_SETUP_ICON "${UMI_DEPLOY_FRAMEWORK_RESOURCES}/brand/umicom.ico")
set(UMI_SETUP_VERSION "${UMI_PACKAGE_VERSION}")
set(UMI_SETUP_SCRIPT "${_work}/UmicomSetup.nsi")
set(UMI_SETUP_OUTPUT_FILE "${_work}/Umicom-Setup.exe")
umicom_render_windows_installer()
if(CMAKE_HOST_WIN32)
    set(_nsis_flags /V3 /NOCD)
else()
    set(_nsis_flags -V3 -NOCD)
endif()
execute_process(COMMAND "${_nsis}" ${_nsis_flags} "${UMI_SETUP_SCRIPT}"
    RESULT_VARIABLE _result OUTPUT_VARIABLE _stdout ERROR_VARIABLE _stderr)
file(WRITE "${UMI_PACKAGE_OUTPUT}/installer-build.log" "${_stdout}\n${_stderr}")
if(NOT _result STREQUAL "0" OR NOT EXISTS "${UMI_SETUP_OUTPUT_FILE}")
    message(FATAL_ERROR
        "The native setup compiler failed. Read ${UMI_PACKAGE_OUTPUT}/installer-build.log. "
        "No new installer has been published.\n${_stderr}")
endif()
# NSIS uses a 32-bit, GTK-independent bootstrap to install the x64 payload.
umicom_windows_require_pe("${UMI_SETUP_OUTPUT_FILE}" "4c01")
umicom_windows_copy_file("${UMI_SETUP_OUTPUT_FILE}" "${UMI_PACKAGE_OUTPUT}/Umicom-Setup.exe")
file(SHA256 "${UMI_PACKAGE_OUTPUT}/Umicom-Setup.exe" _installer_sha)
file(WRITE "${UMI_PACKAGE_OUTPUT}/Umicom-Setup.exe.sha256" "${_installer_sha}  Umicom-Setup.exe\n")
umicom_windows_copy_file("${_work}/payload-sha256.tsv" "${UMI_PACKAGE_OUTPUT}/payload-sha256.tsv")

# A portable folder has the same runtime layout. It contains every application
# selected at build time; the native setup supplies end-user component choices.
set(_portable "${_work}/portable")
file(MAKE_DIRECTORY "${_portable}")
foreach(_component IN ITEMS runtime ${UMI_SETUP_APPS})
    file(COPY "${UMI_SETUP_PAYLOAD_ROOT}/${_component}/" DESTINATION "${_portable}")
endforeach()
execute_process(COMMAND "${CMAKE_COMMAND}" -E tar cf
    "${UMI_PACKAGE_OUTPUT}/Umicom-Portable.zip" --format=zip -- .
    WORKING_DIRECTORY "${_portable}" RESULT_VARIABLE _archive_result)
if(NOT _archive_result STREQUAL "0")
    message(FATAL_ERROR "The installer was built, but the portable archive could not be created")
endif()
file(SHA256 "${UMI_PACKAGE_OUTPUT}/Umicom-Portable.zip" _portable_sha)
file(WRITE "${UMI_PACKAGE_OUTPUT}/Umicom-Portable.zip.sha256" "${_portable_sha}  Umicom-Portable.zip\n")
message(STATUS "Native installer: ${UMI_PACKAGE_OUTPUT}/Umicom-Setup.exe")
message(STATUS "Portable package: ${UMI_PACKAGE_OUTPUT}/Umicom-Portable.zip")
