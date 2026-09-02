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

/*
 * Provide the workbench selection provider bounded length operation used by this module
 * and its client applications.
 */
size_t umi_workbench_selection_provider_bounded_length(
    const char *text,
    size_t capacity)
{
    size_t length = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (length < capacity && text[length] != '\0') ++length;
    return length;
}

/*
 * Check that workbench selection provider text satisfies its contract before another
 * service relies on it.
 */
bool umi_workbench_selection_provider_text_is_valid(
    const char *text,
    size_t capacity)
{
    return text != NULL && capacity > 0U &&
        umi_workbench_selection_provider_bounded_length(
            text, capacity) < capacity;
}

/*
 * Provide the workbench selection provider copy text operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_selection_provider_copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = umi_workbench_selection_provider_bounded_length(
        source, capacity);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench selection provider hash text operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_selection_provider_hash_text(
    uint64_t hash,
    const char *text,
    size_t capacity)
{
    size_t length;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return hash;
    length = umi_workbench_selection_provider_bounded_length(
        text, capacity);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < length; ++index) {
        hash ^= (uint64_t)(unsigned char)text[index];
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

/*
 * Provide the workbench selection provider kind text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_selection_provider_kind_text(
    UmiWorkbenchSelectionProviderKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Provide the workbench selection provider state text operation used by this module and
 * its client applications.
 */
const char *umi_workbench_selection_provider_state_text(
    UmiWorkbenchSelectionProviderRuntimeState state)
{
    /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Provide the workbench selection provider result text operation used by this module and
 * its client applications.
 */
const char *umi_workbench_selection_provider_result_text(
    UmiWorkbenchSelectionProviderResult result)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (result) {
    case UMI_WORKBENCH_SELECTION_PROVIDER_RESULT_RESOLVED: return "resolved";
    case UMI_WORKBENCH_SELECTION_PROVIDER_RESULT_NOT_FOUND: return "not-found";
    case UMI_WORKBENCH_SELECTION_PROVIDER_RESULT_REJECTED: return "rejected";
    case UMI_WORKBENCH_SELECTION_PROVIDER_RESULT_STALE: return "stale";
    case UMI_WORKBENCH_SELECTION_PROVIDER_RESULT_ERROR: return "error";
    default: return "unknown";
    }
}
