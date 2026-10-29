#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomTestReceiptProtocol.cmake
#
# PURPOSE:
#   Check that a built receipt helper supports the driver's command contract.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# Keep protocol parsing separate from process execution so negative regression
# cases prove the reason for refusal rather than accepting any nonzero exit.
# Only transport newlines are normalised; extra output is not a valid handshake.
# Preserve protocol 2 validation for review; aggregate-result admission needs 3.
if(FALSE)
function(umicom_validate_test_receipt_protocol result output reason)
    string(REPLACE "\r\n" "\n" _output "${output}")
    if(NOT "${result}" STREQUAL "0")
        set(_reason "Receipt helper protocol query failed (${result})")
    elseif(NOT _output STREQUAL "UMICOM-TEST-RECEIPTS 2\n")
        set(_reason "Receipt helper does not report protocol 2")
    else()
        set(_reason "")
    endif()
    set("${reason}" "${_reason}" PARENT_SCOPE)
endfunction()
endif()
function(umicom_validate_test_receipt_protocol result output reason)
    string(REPLACE "\r\n" "\n" _output "${output}")
    if(NOT "${result}" STREQUAL "0")
        set(_reason "Receipt helper protocol query failed (${result})")
    elseif(NOT _output STREQUAL "UMICOM-TEST-RECEIPTS 3\n")
        set(_reason "Receipt helper does not report protocol 3")
    else()
        set(_reason "")
    endif()
    set("${reason}" "${_reason}" PARENT_SCOPE)
endfunction()

# Evidence is local to the caller's run. An incompatible executable must not
# prepare exclusions or import receipts. The caller decides how to present the
# failure, while ordinary CTest remains available independently of reuse.
function(umicom_check_test_receipt_protocol program evidence_prefix reason)
    execute_process(COMMAND "${program}" protocol
        RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error
        TIMEOUT 20)
    file(WRITE "${evidence_prefix}.stdout.txt" "${_output}")
    file(WRITE "${evidence_prefix}.stderr.txt" "${_error}")
    file(WRITE "${evidence_prefix}.result.txt" "${_result}\n")
    umicom_validate_test_receipt_protocol("${_result}" "${_output}" _reason)
    if(NOT _reason STREQUAL "")
        string(APPEND _reason
            ". Integrate the complete Framework files, configure again, and rebuild umicom-test-receipts before retrying. Evidence: ${evidence_prefix}")
    endif()
    set("${reason}" "${_reason}" PARENT_SCOPE)
endfunction()
