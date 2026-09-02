/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection/types.c
 *
 * PURPOSE:
 *   Implement bounded text helpers, hashing and stable selection type names.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection/types.h"

#include <string.h>

/*
 * Provide the workbench selection bounded length operation used by this module and its
 * client applications.
 */
size_t umi_workbench_selection_bounded_length(
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
 * Check that workbench selection text satisfies its contract before another service relies
 * on it.
 */
bool umi_workbench_selection_text_is_valid(
    const char *text,
    size_t capacity)
{
    return text != NULL && capacity > 0U &&
        umi_workbench_selection_bounded_length(text, capacity) < capacity;
}

/*
 * Provide the workbench selection copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_selection_copy_text(
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
    length = umi_workbench_selection_bounded_length(source, capacity);
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
 * Provide the workbench selection hash text operation used by this module and its client
 * applications.
 */
uint64_t umi_workbench_selection_hash_text(
    uint64_t hash,
    const char *text,
    size_t capacity)
{
    size_t index;
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return hash;
    length = umi_workbench_selection_bounded_length(text, capacity);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < length; ++index) {
        hash ^= (uint64_t)(unsigned char)text[index];
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

/*
 * Provide the workbench selection kind text operation used by this module and its client
 * applications.
 */
const char *umi_workbench_selection_kind_text(
    UmiWorkbenchSelectionKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
    case UMI_WORKBENCH_SELECTION_GENERIC: return "generic";
    case UMI_WORKBENCH_SELECTION_FILE: return "file";
    case UMI_WORKBENCH_SELECTION_PROJECT: return "project";
    case UMI_WORKBENCH_SELECTION_DIAGNOSTIC: return "diagnostic";
    case UMI_WORKBENCH_SELECTION_SOURCE_CONTROL_CHANGE: return "source-control-change";
    case UMI_WORKBENCH_SELECTION_SOURCE_CONTROL_COMMIT: return "source-control-commit";
    case UMI_WORKBENCH_SELECTION_SOURCE_CONTROL_BRANCH: return "source-control-branch";
    case UMI_WORKBENCH_SELECTION_TEST: return "test";
    case UMI_WORKBENCH_SELECTION_DEBUG_LOCATION: return "debug-location";
    case UMI_WORKBENCH_SELECTION_TERMINAL: return "terminal";
    case UMI_WORKBENCH_SELECTION_AI_MESSAGE: return "ai-message";
    case UMI_WORKBENCH_SELECTION_APPLICATION: return "application";
    case UMI_WORKBENCH_SELECTION_INSTRUMENT: return "instrument";
    case UMI_WORKBENCH_SELECTION_ACCOUNT: return "account";
    case UMI_WORKBENCH_SELECTION_TRADE: return "trade";
    case UMI_WORKBENCH_SELECTION_WORKSPACE: return "workspace";
    case UMI_WORKBENCH_SELECTION_MEDIA: return "media";
    default: return "unknown";
    }
}

/*
 * Provide the workbench selection activation text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_selection_activation_text(
    UmiWorkbenchSelectionActivation activation)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (activation) {
    case UMI_WORKBENCH_SELECTION_ACTIVATION_SELECT: return "select";
    case UMI_WORKBENCH_SELECTION_ACTIVATION_OPEN: return "open";
    case UMI_WORKBENCH_SELECTION_ACTIVATION_NAVIGATE: return "navigate";
    case UMI_WORKBENCH_SELECTION_ACTIVATION_FOCUS: return "focus";
    case UMI_WORKBENCH_SELECTION_ACTIVATION_PREVIEW: return "preview";
    default: return "unknown";
    }
}

/*
 * Provide the workbench selection state text operation used by this module and its client
 * applications.
 */
const char *umi_workbench_selection_state_text(
    UmiWorkbenchSelectionState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
    case UMI_WORKBENCH_SELECTION_STATE_CREATED: return "created";
    case UMI_WORKBENCH_SELECTION_STATE_RESOLVED: return "resolved";
    case UMI_WORKBENCH_SELECTION_STATE_PUBLISHED: return "published";
    case UMI_WORKBENCH_SELECTION_STATE_REJECTED: return "rejected";
    case UMI_WORKBENCH_SELECTION_STATE_STALE: return "stale";
    default: return "unknown";
    }
}
