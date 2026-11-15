/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/types.c
 *
 * PURPOSE:
 *   Implement stable diagnostic text for operational AI coding state.
 *   Validate bounded event and snapshot labels before observation storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/types.h"
#include "umicom/base/snapshot_validation.h"

/*
 * Provide the ai coding runtime state text operation used by this module and its client
 * applications.
 */
const char *umi_ai_coding_runtime_state_text(UmiAiCodingRuntimeState state)
{
    /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Provide the ai coding validation kind text operation used by this module and its client
 * applications.
 */
const char *umi_ai_coding_validation_kind_text(UmiAiCodingValidationKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Provide the ai coding event kind text operation used by this module and its client
 * applications.
 */
const char *umi_ai_coding_event_kind_text(UmiAiCodingEventKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
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

/* Observation storage preserves historical scalar metadata. Only text bounds
 * are checked here; a saved status or revision cannot authorize execution. */
UmiStatus UmiAiCodingEventValidateText(const UmiAiCodingEvent *event)
{
    static const UmiSnapshotTextField fields[] = {
        {"task_id", offsetof(UmiAiCodingEvent, task_id), sizeof(event->task_id), 0},
        {"message", offsetof(UmiAiCodingEvent, message), sizeof(event->message), 0}
    };
    return UmiSnapshotValidateTextFields(event, sizeof(*event), fields,
        sizeof fields / sizeof fields[0], NULL);
}

UmiStatus UmiAiCodingAgentSnapshotValidateText(const UmiAiCodingAgentSnapshot *snapshot)
{
    static const UmiSnapshotTextField fields[] = {
        {"task_id", offsetof(UmiAiCodingAgentSnapshot, task_id), sizeof(snapshot->task_id), 0},
        {"request_id", offsetof(UmiAiCodingAgentSnapshot, request_id), sizeof(snapshot->request_id), 0},
        {"provider_id", offsetof(UmiAiCodingAgentSnapshot, provider_id), sizeof(snapshot->provider_id), 0},
        {"model_id", offsetof(UmiAiCodingAgentSnapshot, model_id), sizeof(snapshot->model_id), 0},
        {"patch_id", offsetof(UmiAiCodingAgentSnapshot, patch_id), sizeof(snapshot->patch_id), 0}
    };
    return UmiSnapshotValidateTextFields(snapshot, sizeof(*snapshot), fields,
        sizeof fields / sizeof fields[0], NULL);
}
