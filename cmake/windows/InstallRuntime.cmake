#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/windows/InstallRuntime.cmake
#
# PURPOSE:
#   Extend existing install/CPack components with their complete private runtime.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include("${CMAKE_CURRENT_LIST_DIR}/DeploymentCommon.cmake")
if(NOT UMI_DEPLOY_INSTALL_ROOT)
    message(FATAL_ERROR "A destination prefix is required to install the runtime")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" "-DUMI_DEPLOY_CONFIG=${UMI_DEPLOY_CONFIG}"
    -P "${CMAKE_CURRENT_LIST_DIR}/StageRuntime.cmake" RESULT_VARIABLE _stage_result)
if(NOT _stage_result STREQUAL "0")
    message(FATAL_ERROR "The Windows runtime failed validation and has not been installed")
endif()
include("${UMI_DEPLOY_CONFIG}")
include("${UMI_DEPLOY_WORK}/${UMI_DEPLOY_TARGET}.runtime.cmake")
foreach(_record IN LISTS UMI_REPORT_FILES)
    string(REPLACE "|" ";" _fields "${_record}")
    list(LENGTH _fields _count)
    if(NOT _count EQUAL 3)
        message(FATAL_ERROR "Malformed runtime ownership record")
    endif()
    list(GET _fields 0 _relative)
    list(GET _fields 1 _source)
    list(GET _fields 2 _expected_hash)
    file(SHA256 "${_source}" _actual_hash)
    if(NOT _actual_hash STREQUAL _expected_hash)
        message(FATAL_ERROR "A runtime file changed after staging: ${_source}")
    endif()
    umicom_windows_require_relative("${_relative}")
    umicom_windows_copy_file("${_source}" "${UMI_DEPLOY_INSTALL_ROOT}/${_relative}")
endforeach()
get_filename_component(_name "${UMI_REPORT_EXECUTABLE}" NAME)
umicom_windows_copy_file("${UMI_REPORT_EXECUTABLE}" "${UMI_DEPLOY_INSTALL_ROOT}/bin/${_name}")
