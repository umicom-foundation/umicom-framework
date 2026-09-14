#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomSourceContractDispatch.cmake
# PURPOSE: Delegate legacy CMake audit entry points to the native implementation.
# Author: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------

if(NOT DEFINED UMICOM_SOURCE_CONTRACT_TOOL OR NOT EXISTS "${UMICOM_SOURCE_CONTRACT_TOOL}")
    if(CMAKE_HOST_WIN32)
        set(_umicom_contract_suffix ".exe")
    else()
        set(_umicom_contract_suffix "")
    endif()
    set(UMICOM_SOURCE_CONTRACT_TOOL "${CMAKE_BINARY_DIR}/bin/umicom-source-contracts${_umicom_contract_suffix}")
endif()
if(NOT EXISTS "${UMICOM_SOURCE_CONTRACT_TOOL}")
    message(FATAL_ERROR
        "Native source auditor is missing. Build the umicom-source-contracts target first, "
        "or provide -DUMICOM_SOURCE_CONTRACT_TOOL=<absolute executable path>.")
endif()
set(_umicom_contract_arguments "${UMICOM_CONTRACT_MODE}")
foreach(_umicom_contract_root IN LISTS UMICOM_AUDIT_ROOTS)
    list(APPEND _umicom_contract_arguments --root "${_umicom_contract_root}")
endforeach()
execute_process(COMMAND "${UMICOM_SOURCE_CONTRACT_TOOL}" ${_umicom_contract_arguments}
    RESULT_VARIABLE _umicom_contract_result)
if(NOT "${_umicom_contract_result}" STREQUAL "0")
    message(FATAL_ERROR "${UMICOM_CONTRACT_OWNER}: native source audit failed (${_umicom_contract_result}).")
endif()
