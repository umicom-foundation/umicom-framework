/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/cvm_status.h
 *
 * PURPOSE:
 *   Share compiler status translation between optional execution and source-only adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_TOOLCHAIN_CVM_STATUS_INTERNAL_H
#define UMICOM_TOOLCHAIN_CVM_STATUS_INTERNAL_H
#include "umicom/base/status.h"
#include "umicom_c/umicom_c.h"

static inline UmiStatus UmiCvmTranslateStatus(UmcStatus status)
{
    switch (status) {
    case UMC_OK: return UMI_STATUS_OK;
    case UMC_INVALID_ARGUMENT: return UMI_STATUS_INVALID_ARGUMENT;
    case UMC_SYNTAX_ERROR:
    case UMC_LITERAL_RANGE:
    case UMC_BAD_BYTECODE: return UMI_STATUS_PARSE_ERROR;
    case UMC_SOURCE_TOO_LARGE:
    case UMC_PROGRAM_TOO_LARGE:
    case UMC_DEPTH_LIMIT:
    case UMC_STACK_OVERFLOW:
    case UMC_FUEL_EXHAUSTED:
    /* Optional SDK extension: keep older supported headers compilable while
     * translating the new compiler bound through the shared status contract. */
#ifdef UMC_LOCAL_INTEGER_PROFILE
    case UMC_LOCAL_CAPACITY:
#endif
    case UMC_BUFFER_TOO_SMALL: return UMI_STATUS_CAPACITY_EXCEEDED;
    case UMC_ARITHMETIC_ERROR: return UMI_STATUS_INVALID_STATE;
    default: return UMI_STATUS_INTERNAL_ERROR;
    }
}
#endif
