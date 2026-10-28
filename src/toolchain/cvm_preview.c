/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/cvm_preview.c
 *
 * PURPOSE:
 *   Expose bounded source preview with explicit result ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/cvm_preview.h"

static UmiStatus Translate(UmcStatus status)
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
    case UMC_BUFFER_TOO_SMALL: return UMI_STATUS_CAPACITY_EXCEEDED;
    case UMC_ARITHMETIC_ERROR: return UMI_STATUS_INVALID_STATE;
    default: return UMI_STATUS_INTERNAL_ERROR;
    }
}
UmiStatus umi_cvm_preview(const char *source, size_t length, size_t fuel,
    UmiCvmPreviewWorkspace *workspace, size_t workspace_size,
    UmiCvmPreviewResult *result, size_t result_size)
{
    if (workspace == NULL || result == NULL ||
        workspace_size != sizeof *workspace || result_size != sizeof *result)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* The Framework owns presentation-facing state transitions. Applications
     * never infer success from an old value when a later preview is rejected. */
    result->completed = false;
    result->phase = UMI_CVM_COMPILING;
    result->instruction_count = 0U;
    result->execution.value = 0;
    result->execution.instructions_executed = 0U;
    UmcStatus status = umc_compile(source, length, workspace->instructions,
        UMC_PROGRAM_LIMIT, &result->instruction_count, &result->diagnostic);
    if (status != UMC_OK) return Translate(status);
    result->phase = UMI_CVM_EXECUTING;
    status = umc_execute(workspace->instructions, result->instruction_count,
        fuel, &result->execution, &result->diagnostic);
    umc_locate_diagnostic(source, length, &result->diagnostic);
    if (status != UMC_OK) return Translate(status);
    result->phase = UMI_CVM_COMPLETE;
    result->completed = true;
    return UMI_STATUS_OK;
}
