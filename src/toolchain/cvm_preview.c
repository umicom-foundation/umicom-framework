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
#include "umicom/toolchain/cvm_trace.h"

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
/* Preserve the earlier adapter for engineering review. The shared request
 * lifecycle below keeps ordinary and traced previews aligned and reuses the
 * Umicom C runtime for all execution, including failed attempted steps. */
#if 0
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
#endif

static UmiStatus Preview(const char *source, size_t length, size_t fuel,
    UmiCvmPreviewWorkspace *workspace, size_t workspace_size,
    UmiCvmPreviewResult *result, size_t result_size, UmiCvmPreviewTrace *trace)
{
    if (workspace == NULL || result == NULL ||
        workspace_size != sizeof *workspace || result_size != sizeof *result)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* The Framework owns presentation-facing state transitions. Applications
     * never infer success from an old value when a later preview is rejected. */
    if (trace != NULL) trace->count = 0U;
    result->completed = false;
    result->phase = UMI_CVM_COMPILING;
    result->instruction_count = 0U;
    result->execution.value = 0;
    result->execution.instructions_executed = 0U;
    UmcStatus status = umc_compile(source, length, workspace->instructions,
        UMC_PROGRAM_LIMIT, &result->instruction_count, &result->diagnostic);
    if (status != UMC_OK) return Translate(status);
    result->phase = UMI_CVM_EXECUTING;
    if (trace != NULL) {
        status = umc_execute_trace(workspace->instructions, result->instruction_count,
            fuel, &result->execution, trace->steps, UMC_PROGRAM_LIMIT, &trace->count,
            &result->diagnostic);
    } else {
        status = umc_execute(workspace->instructions, result->instruction_count,
            fuel, &result->execution, &result->diagnostic);
    }
    umc_locate_diagnostic(source, length, &result->diagnostic);
    if (status != UMC_OK) return Translate(status);
    result->phase = UMI_CVM_COMPLETE;
    result->completed = true;
    return UMI_STATUS_OK;
}

UmiStatus umi_cvm_preview(const char *source, size_t length, size_t fuel,
    UmiCvmPreviewWorkspace *workspace, size_t workspace_size,
    UmiCvmPreviewResult *result, size_t result_size)
{
    return Preview(source, length, fuel, workspace, workspace_size, result, result_size, NULL);
}

UmiStatus umi_cvm_preview_traced(const char *source, size_t length, size_t fuel,
    UmiCvmPreviewWorkspace *workspace, size_t workspace_size,
    UmiCvmPreviewResult *result, size_t result_size,
    UmiCvmPreviewTrace *trace, size_t trace_size)
{
    if (trace == NULL || trace_size != sizeof *trace) return UMI_STATUS_INVALID_ARGUMENT;
    return Preview(source, length, fuel, workspace, workspace_size, result, result_size, trace);
}
