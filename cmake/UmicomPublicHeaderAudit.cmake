#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomPublicHeaderAudit.cmake
#
# PURPOSE:
#   Audit every public header for a unique matching include guard and the
#   human-readable Umicom file comment required by the coding convention.
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
if(DEFINED UMICOM_HEADER_ROOTS AND NOT UMICOM_HEADER_ROOTS STREQUAL "")
    string(REPLACE "|" ";" UMICOM_AUDIT_ROOTS "${UMICOM_HEADER_ROOTS}")
# Use the stable identifier comparison to choose the matching record or policy.
elseif(DEFINED UMICOM_HEADER_ROOT AND NOT UMICOM_HEADER_ROOT STREQUAL "")
    set(UMICOM_AUDIT_ROOTS "${UMICOM_HEADER_ROOT}")
else()
    message(FATAL_ERROR "UMICOM_HEADER_ROOTS must name at least one source directory.")
endif()
set(UMICOM_CONTRACT_OWNER "${UMICOM_HEADER_OWNER}")
set(UMICOM_CONTRACT_MODE headers)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSourceContractDispatch.cmake")
