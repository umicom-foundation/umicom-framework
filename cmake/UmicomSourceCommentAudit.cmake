#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomSourceCommentAudit.cmake
#
# PURPOSE:
#   Audit C implementation and include-fragment files for the human-readable
#   purpose, author and licence information required by the coding convention.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------

# Validation belongs to the shared native C implementation. This adapter only
# preserves the established CMake command interface and propagates its result.
if(DEFINED UMICOM_SOURCE_ROOTS AND NOT UMICOM_SOURCE_ROOTS STREQUAL "")
    string(REPLACE "|" ";" UMICOM_AUDIT_ROOTS "${UMICOM_SOURCE_ROOTS}")
elseif(DEFINED UMICOM_SOURCE_ROOT AND NOT UMICOM_SOURCE_ROOT STREQUAL "")
    set(UMICOM_AUDIT_ROOTS "${UMICOM_SOURCE_ROOT}")
else()
    message(FATAL_ERROR "UMICOM_SOURCE_ROOTS must name at least one source directory.")
endif()
set(UMICOM_CONTRACT_OWNER "${UMICOM_SOURCE_OWNER}")
set(UMICOM_CONTRACT_MODE comments)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSourceContractDispatch.cmake")
