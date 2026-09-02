/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/decoders/initialize.c
 *
 * PURPOSE:
 *   Decode DAP initialize response capabilities into bounded Framework runtime records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/decoders/initialize.h"

#include <stdio.h>
#include <string.h>

/*
 * Initialise debug runtime decode from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_debug_runtime_decode_initialize(
    const char *json,
    UmiDebugRuntimeCapabilities *out_result)
{
    UmiLanguageRuntimeJsonDocument document;
    int body;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (json == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_result, 0, sizeof(*out_result));

    status = umi_language_runtime_json_parse(json, &document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    body = umi_debug_runtime_decoder_body_token(&document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (body < 0) return UMI_STATUS_PARSE_ERROR;

#define CAP(field, key) \
    out_result->field = umi_debug_runtime_decoder_optional_bool( \
        &document, body, key, 0)
    CAP(supports_configuration_done, "supportsConfigurationDoneRequest");
    CAP(supports_function_breakpoints, "supportsFunctionBreakpoints");
    CAP(supports_conditional_breakpoints, "supportsConditionalBreakpoints");
    CAP(supports_hit_conditions, "supportsHitConditionalBreakpoints");
    CAP(supports_evaluate_for_hovers, "supportsEvaluateForHovers");
    CAP(supports_set_variable, "supportsSetVariable");
    CAP(supports_restart, "supportsRestartRequest");
    CAP(supports_terminate_request, "supportsTerminateRequest");
    CAP(supports_modules_request, "supportsModulesRequest");
    CAP(supports_read_memory_request, "supportsReadMemoryRequest");
    CAP(supports_disassemble_request, "supportsDisassembleRequest");
    CAP(supports_step_back, "supportsStepBack");
    CAP(supports_data_breakpoints, "supportsDataBreakpoints");
    CAP(supports_instruction_breakpoints, "supportsInstructionBreakpoints");
    CAP(supports_exception_filter_options, "supportsExceptionFilterOptions");
    CAP(supports_write_memory_request, "supportsWriteMemoryRequest");
    CAP(supports_set_expression, "supportsSetExpression");
    CAP(supports_restart_frame, "supportsRestartFrame");
    CAP(supports_completions_request, "supportsCompletionsRequest");
    CAP(supports_cancel_request, "supportsCancelRequest");
    CAP(supports_loaded_sources_request, "supportsLoadedSourcesRequest");
    CAP(supports_terminate_threads_request, "supportsTerminateThreadsRequest");
    CAP(supports_goto_targets_request, "supportsGotoTargetsRequest");
#undef CAP
    return UMI_STATUS_OK;
}
