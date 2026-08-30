/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/types.c
 *
 * PURPOSE:
 *   Implement stable diagnostic text for operational AI coding state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/types.h"

const char *umi_ai_coding_runtime_state_text(UmiAiCodingRuntimeState state)
{
    switch (state) {
        case UMI_AI_CODING_RUNTIME_IDLE: return "idle";
        case UMI_AI_CODING_RUNTIME_PREPARING: return "preparing";
        case UMI_AI_CODING_RUNTIME_MATERIALISING_CONTEXT:
            return "materialising-context";
        case UMI_AI_CODING_RUNTIME_GENERATING: return "generating";
        case UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED: return "review-required";
        case UMI_AI_CODING_RUNTIME_APPLYING: return "applying";
        case UMI_AI_CODING_RUNTIME_VALIDATING: return "validating";
        case UMI_AI_CODING_RUNTIME_REPAIRING: return "repairing";
        case UMI_AI_CODING_RUNTIME_COMPLETED: return "completed";
        case UMI_AI_CODING_RUNTIME_CANCELLED: return "cancelled";
        case UMI_AI_CODING_RUNTIME_FAILED: return "failed";
        default: return "unknown";
    }
}

const char *umi_ai_coding_validation_kind_text(UmiAiCodingValidationKind kind)
{
    switch (kind) {
        case UMI_AI_CODING_VALIDATION_CUSTOM: return "custom";
        case UMI_AI_CODING_VALIDATION_CONFIGURE: return "configure";
        case UMI_AI_CODING_VALIDATION_BUILD: return "build";
        case UMI_AI_CODING_VALIDATION_TEST: return "test";
        case UMI_AI_CODING_VALIDATION_LINT: return "lint";
        case UMI_AI_CODING_VALIDATION_FORMAT_CHECK: return "format-check";
        case UMI_AI_CODING_VALIDATION_PACKAGE: return "package";
        default: return "unknown";
    }
}

const char *umi_ai_coding_event_kind_text(UmiAiCodingEventKind kind)
{
    switch (kind) {
        case UMI_AI_CODING_EVENT_STATE: return "state";
        case UMI_AI_CODING_EVENT_CONTEXT: return "context";
        case UMI_AI_CODING_EVENT_PROVIDER: return "provider";
        case UMI_AI_CODING_EVENT_PATCH: return "patch";
        case UMI_AI_CODING_EVENT_VALIDATION: return "validation";
        case UMI_AI_CODING_EVENT_ROLLBACK: return "rollback";
        case UMI_AI_CODING_EVENT_MESSAGE: return "message";
        default: return "unknown";
    }
}
