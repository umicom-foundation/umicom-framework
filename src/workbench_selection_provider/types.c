/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection_provider/types.c
 *
 * PURPOSE:
 *   Implement bounded text helpers, hashing and stable provider state names.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection_provider/types.h"

#include <string.h>

size_t umi_workbench_selection_provider_bounded_length(
    const char *text,
    size_t capacity)
{
    size_t length = 0U;
    if (text == NULL) return 0U;
    while (length < capacity && text[length] != '\0') ++length;
    return length;
}

bool umi_workbench_selection_provider_text_is_valid(
    const char *text,
    size_t capacity)
{
    return text != NULL && capacity > 0U &&
        umi_workbench_selection_provider_bounded_length(
            text, capacity) < capacity;
}

UmiStatus umi_workbench_selection_provider_copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = umi_workbench_selection_provider_bounded_length(
        source, capacity);
    if (length >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (length > 0U) memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

uint64_t umi_workbench_selection_provider_hash_text(
    uint64_t hash,
    const char *text,
    size_t capacity)
{
    size_t length;
    size_t index;
    if (text == NULL) return hash;
    length = umi_workbench_selection_provider_bounded_length(
        text, capacity);
    for (index = 0U; index < length; ++index) {
        hash ^= (uint64_t)(unsigned char)text[index];
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

const char *umi_workbench_selection_provider_kind_text(
    UmiWorkbenchSelectionProviderKind kind)
{
    switch (kind) {
    case UMI_WORKBENCH_SELECTION_PROVIDER_GENERIC: return "generic";
    case UMI_WORKBENCH_SELECTION_PROVIDER_PROJECT: return "project";
    case UMI_WORKBENCH_SELECTION_PROVIDER_FILE: return "file";
    case UMI_WORKBENCH_SELECTION_PROVIDER_PROBLEM: return "problem";
    case UMI_WORKBENCH_SELECTION_PROVIDER_SOURCE_CONTROL_CHANGE: return "source-control-change";
    case UMI_WORKBENCH_SELECTION_PROVIDER_SOURCE_CONTROL_COMMIT: return "source-control-commit";
    case UMI_WORKBENCH_SELECTION_PROVIDER_SOURCE_CONTROL_BRANCH: return "source-control-branch";
    case UMI_WORKBENCH_SELECTION_PROVIDER_TEST: return "test";
    case UMI_WORKBENCH_SELECTION_PROVIDER_AI: return "ai";
    case UMI_WORKBENCH_SELECTION_PROVIDER_APPLICATION: return "application";
    case UMI_WORKBENCH_SELECTION_PROVIDER_INSTRUMENT: return "instrument";
    case UMI_WORKBENCH_SELECTION_PROVIDER_ACCOUNT: return "account";
    case UMI_WORKBENCH_SELECTION_PROVIDER_TRADE: return "trade";
    case UMI_WORKBENCH_SELECTION_PROVIDER_WORKSPACE: return "workspace";
    case UMI_WORKBENCH_SELECTION_PROVIDER_MEDIA: return "media";
    default: return "unknown";
    }
}

const char *umi_workbench_selection_provider_state_text(
    UmiWorkbenchSelectionProviderRuntimeState state)
{
    switch (state) {
    case UMI_WORKBENCH_SELECTION_PROVIDER_CREATED: return "created";
    case UMI_WORKBENCH_SELECTION_PROVIDER_READY: return "ready";
    case UMI_WORKBENCH_SELECTION_PROVIDER_ACTIVE: return "active";
    case UMI_WORKBENCH_SELECTION_PROVIDER_SUSPENDED: return "suspended";
    case UMI_WORKBENCH_SELECTION_PROVIDER_FAILED: return "failed";
    case UMI_WORKBENCH_SELECTION_PROVIDER_STOPPED: return "stopped";
    default: return "unknown";
    }
}

const char *umi_workbench_selection_provider_result_text(
    UmiWorkbenchSelectionProviderResult result)
{
    switch (result) {
    case UMI_WORKBENCH_SELECTION_PROVIDER_RESULT_RESOLVED: return "resolved";
    case UMI_WORKBENCH_SELECTION_PROVIDER_RESULT_NOT_FOUND: return "not-found";
    case UMI_WORKBENCH_SELECTION_PROVIDER_RESULT_REJECTED: return "rejected";
    case UMI_WORKBENCH_SELECTION_PROVIDER_RESULT_STALE: return "stale";
    case UMI_WORKBENCH_SELECTION_PROVIDER_RESULT_ERROR: return "error";
    default: return "unknown";
    }
}
