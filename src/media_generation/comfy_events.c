/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_events.c
 *
 * PURPOSE:
 *   Interpret ComfyUI text observations without confusing queue activity, UI updates or local cancellation with job completion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_events.h"
#include "umicom/language_runtime/json_tree.h"
#include <string.h>

static UmiStatus member(const UmiJsonTree *tree, int object, const char *name, int *out)
{
    UmiStatus status = UmiJsonTreeMember(tree, object, name, out);
    return status == UMI_STATUS_NOT_FOUND ? UMI_STATUS_PARSE_ERROR : status;
}
static UmiStatus text(const UmiJsonTree *tree, int object, const char *name, char *out, size_t capacity)
{
    int node = -1;
    UmiStatus status = member(tree, object, name, &node);
    if (status != UMI_STATUS_OK) return status;
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING) return UMI_STATUS_PARSE_ERROR;
    status = UmiJsonTreeText(tree, node, out, capacity);
    if (status == UMI_STATUS_OK && out[0] == '\0') return UMI_STATUS_PARSE_ERROR;
    return status;
}
static UmiStatus number(const UmiJsonTree *tree, int object, const char *name, uint64_t *out)
{
    int node = -1;
    UmiStatus status = member(tree, object, name, &node);
    if (status != UMI_STATUS_OK) return status;
    int64_t value = 0;
    status = UmiJsonTreeInteger(tree, node, &value);
    if (status != UMI_STATUS_OK) return status;
    if (value < 0) return UMI_STATUS_PARSE_ERROR;
    *out = (uint64_t)value;
    return UMI_STATUS_OK;
}
static UmiStatus decode(const UmiJsonTree *tree, const UmiCancellationToken *cancel, UmiComfyEvent *out)
{
    if (UmiJsonTreeKind(tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) return UMI_STATUS_PARSE_ERROR;
    char type[64];
    UmiStatus status = text(tree, 0, "type", type, sizeof(type));
    if (status != UMI_STATUS_OK) return status;
    int data = -1;
    status = member(tree, 0, "data", &data);
    if (status != UMI_STATUS_OK) return status;
    if (UmiJsonTreeKind(tree, data) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) return UMI_STATUS_PARSE_ERROR;
    static const struct { const char *name; UmiComfyEventKind kind; } kinds[] = {
        {"status", UMI_COMFY_EVENT_QUEUE}, {"execution_start", UMI_COMFY_EVENT_START},
        {"progress", UMI_COMFY_EVENT_PROGRESS}, {"executing", UMI_COMFY_EVENT_EXECUTING},
        {"executed", UMI_COMFY_EVENT_UI_OUTPUT}, {"execution_cached", UMI_COMFY_EVENT_CACHED},
        {"execution_success", UMI_COMFY_EVENT_SUCCESS}, {"execution_error", UMI_COMFY_EVENT_ERROR},
        {"execution_interrupted", UMI_COMFY_EVENT_INTERRUPTED}
    };
    for (size_t i = 0; i < sizeof(kinds)/sizeof(kinds[0]); ++i)
        if (strcmp(type, kinds[i].name) == 0) { out->kind = kinds[i].kind; break; }
    if (out->kind == 0) return UMI_STATUS_NOT_IMPLEMENTED;
    if (out->kind == UMI_COMFY_EVENT_QUEUE) {
        int info = -1;
        status = member(tree, data, "exec_info", &info);
        if (status != UMI_STATUS_OK) return status;
        if (UmiJsonTreeKind(tree, info) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) return UMI_STATUS_PARSE_ERROR;
        return number(tree, info, "queue_remaining", &out->queue_remaining);
    }
    status = text(tree, data, "prompt_id", out->prompt_id, sizeof(out->prompt_id));
    if (status != UMI_STATUS_OK) return status;
    if (out->kind == UMI_COMFY_EVENT_PROGRESS) {
        status = text(tree, data, "node", out->node_id, sizeof(out->node_id));
        if (status != UMI_STATUS_OK) return status;
        status = number(tree, data, "value", &out->value);
        if (status != UMI_STATUS_OK) return status;
        status = number(tree, data, "max", &out->maximum);
        if (status != UMI_STATUS_OK) return status;
        return out->value <= out->maximum ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
    }
    if (out->kind == UMI_COMFY_EVENT_EXECUTING) {
        int node = -1;
        status = member(tree, data, "node", &node);
        if (status != UMI_STATUS_OK) return status;
        if (UmiJsonTreeIsNull(tree, node)) { out->node_is_null = 1; return UMI_STATUS_OK; }
        return text(tree, data, "node", out->node_id, sizeof(out->node_id));
    }
    if (out->kind == UMI_COMFY_EVENT_UI_OUTPUT) {
        status = text(tree, data, "node", out->node_id, sizeof(out->node_id));
        if (status != UMI_STATUS_OK) return status;
        int output = -1;
        status = member(tree, data, "output", &output);
        if (status != UMI_STATUS_OK) return status;
        return UmiJsonTreeKind(tree, output) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
    }
    if (out->kind == UMI_COMFY_EVENT_CACHED) {
        int nodes = -1;
        status = member(tree, data, "nodes", &nodes);
        if (status != UMI_STATUS_OK) return status;
        if (UmiJsonTreeKind(tree, nodes) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY) return UMI_STATUS_PARSE_ERROR;
        if (UmiJsonTreeCount(tree, nodes) > 1024U) return UMI_STATUS_CAPACITY_EXCEEDED;
        for (int node = UmiJsonTreeFirst(tree, nodes); node >= 0; node = UmiJsonTreeNext(tree, node)) {
            if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
            char id[UMI_PROVIDER_TASK_REMOTE_ID_CAPACITY];
            if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING) return UMI_STATUS_PARSE_ERROR;
            status = UmiJsonTreeText(tree, node, id, sizeof(id));
            if (status != UMI_STATUS_OK) return status;
            if (id[0] == '\0') return UMI_STATUS_PARSE_ERROR;
        }
    }
    if (out->kind == UMI_COMFY_EVENT_INTERRUPTED)
        return text(tree, data, "node_id", out->node_id, sizeof(out->node_id));
    return UMI_STATUS_OK;
}
UmiStatus UmiComfyEventDecode(const void *bytes, size_t length,
    const UmiCancellationToken *cancel, UmiComfyEvent *out)
{
    if (bytes == NULL || length == 0 || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_COMFY_EVENT_MAX_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiJsonTreeLimits limits = {UMI_COMFY_EVENT_MAX_BYTES, 32768U, 32U};
    UmiJsonTree *tree = NULL;
    UmiStatus status = UmiJsonTreeCreate(bytes, length, &limits, cancel, &tree);
    if (status != UMI_STATUS_OK) return status;
    status = UmiJsonTreeValidateUniqueKeys(tree, 0, cancel);
    UmiComfyEvent staged = {0};
    if (status == UMI_STATUS_OK) status = decode(tree, cancel, &staged);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel)) status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK) *out = staged;
    UmiJsonTreeDestroy(tree);
    return status;
}
UmiStatus UmiComfyTaskObserveJson(const UmiProviderTaskReceipt *receipt,
    uint64_t expected_revision, const void *bytes, size_t length,
    const UmiCancellationToken *cancel, UmiProviderTaskReceipt *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiProviderTaskValidate(receipt);
    if (status != UMI_STATUS_OK) return status;
    if (receipt->revision != expected_revision) return UMI_STATUS_BUSY;
    if (strcmp(receipt->provider_id, "comfyui") != 0 ||
        strcmp(receipt->context.operation_id, UMI_COMFY_TASK_OPERATION) != 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (receipt->remote_id[0] == '\0') return UMI_STATUS_INVALID_STATE;
    UmiComfyEvent event;
    status = UmiComfyEventDecode(bytes, length, cancel, &event);
    if (status != UMI_STATUS_OK) return status;
    if (event.kind == UMI_COMFY_EVENT_QUEUE) return UMI_STATUS_UNAVAILABLE;
    if (strcmp(event.prompt_id, receipt->remote_id) != 0) return UMI_STATUS_NOT_FOUND;
    UmiProviderTaskEvent transition;
    switch (event.kind) {
        case UMI_COMFY_EVENT_START:
        case UMI_COMFY_EVENT_PROGRESS: transition = UMI_PROVIDER_TASK_OBSERVE_RUNNING; break;
        case UMI_COMFY_EVENT_SUCCESS: transition = UMI_PROVIDER_TASK_OBSERVE_SUCCESS; break;
        case UMI_COMFY_EVENT_ERROR:
        case UMI_COMFY_EVENT_INTERRUPTED: transition = UMI_PROVIDER_TASK_OBSERVE_FAILURE; break;
        default: return UMI_STATUS_UNAVAILABLE;
    }
    /* Shared state admission owns no-op, stale and terminal transition rules.
     * The host still needs a checked store commit and provider-scoped transport. */
    return UmiProviderTaskAdvance(receipt, expected_revision, transition, out);
}
