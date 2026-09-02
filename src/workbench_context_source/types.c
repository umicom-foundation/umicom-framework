/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_source/types.c
 *
 * PURPOSE:
 *   Implement bounded text helpers, hashing and stable text conversion for live interaction source contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_source/types.h"

#include <string.h>

/*
 * Provide the workbench context source bounded length operation used by this module and
 * its client applications.
 */
size_t umi_workbench_context_source_bounded_length(
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
 * Check that workbench context source text satisfies its contract before another service
 * relies on it.
 */
bool umi_workbench_context_source_text_is_valid(
    const char *text,
    size_t capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || capacity == 0U) return false;
    return umi_workbench_context_source_bounded_length(
        text, capacity) < capacity;
}

/*
 * Provide the workbench context source copy text operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_source_copy_text(
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
    length = umi_workbench_context_source_bounded_length(
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
 * Provide the workbench context source hash text operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_source_hash_text(
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
    length = umi_workbench_context_source_bounded_length(
        text, capacity);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < length; ++index) {
        hash ^= (uint64_t)(unsigned char)text[index];
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

/*
 * Provide the workbench context source kind text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_context_source_kind_text(
    UmiWorkbenchContextSourceKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
    case UMI_WORKBENCH_CONTEXT_SOURCE_GENERIC: return "generic";
    case UMI_WORKBENCH_CONTEXT_SOURCE_EDITOR: return "editor";
    case UMI_WORKBENCH_CONTEXT_SOURCE_PROJECT_EXPLORER: return "project-explorer";
    case UMI_WORKBENCH_CONTEXT_SOURCE_PROBLEMS: return "problems";
    case UMI_WORKBENCH_CONTEXT_SOURCE_SOURCE_CONTROL: return "source-control";
    case UMI_WORKBENCH_CONTEXT_SOURCE_TEST_EXPLORER: return "test-explorer";
    case UMI_WORKBENCH_CONTEXT_SOURCE_DEBUGGER: return "debugger";
    case UMI_WORKBENCH_CONTEXT_SOURCE_TERMINAL: return "terminal";
    case UMI_WORKBENCH_CONTEXT_SOURCE_AI: return "ai";
    case UMI_WORKBENCH_CONTEXT_SOURCE_APPLICATION_LAUNCHER: return "application-launcher";
    case UMI_WORKBENCH_CONTEXT_SOURCE_WATCHLIST: return "watchlist";
    case UMI_WORKBENCH_CONTEXT_SOURCE_ACCOUNT_SELECTOR: return "account-selector";
    case UMI_WORKBENCH_CONTEXT_SOURCE_TRADE_BLOTTER: return "trade-blotter";
    case UMI_WORKBENCH_CONTEXT_SOURCE_CHART: return "chart";
    case UMI_WORKBENCH_CONTEXT_SOURCE_RISK: return "risk";
    case UMI_WORKBENCH_CONTEXT_SOURCE_MEDIA: return "media";
    default: return "unknown";
    }
}

/*
 * Provide the workbench context source trigger text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_context_source_trigger_text(
    UmiWorkbenchContextSourceTrigger trigger)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (trigger) {
    case UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_ACTIVATE: return "activate";
    case UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_SELECT: return "select";
    case UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_CARET: return "caret";
    case UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_RANGE: return "range";
    case UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_CHANGE: return "change";
    case UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_FOCUS: return "focus";
    case UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_OPEN: return "open";
    case UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_NAVIGATE: return "navigate";
    default: return "unknown";
    }
}

/*
 * Provide the workbench context source state text operation used by this module and its
 * client applications.
 */
const char *umi_workbench_context_source_state_text(
    UmiWorkbenchContextSourceState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
    case UMI_WORKBENCH_CONTEXT_SOURCE_STATE_CREATED: return "created";
    case UMI_WORKBENCH_CONTEXT_SOURCE_STATE_READY: return "ready";
    case UMI_WORKBENCH_CONTEXT_SOURCE_STATE_ACTIVE: return "active";
    case UMI_WORKBENCH_CONTEXT_SOURCE_STATE_SUSPENDED: return "suspended";
    case UMI_WORKBENCH_CONTEXT_SOURCE_STATE_FAILED: return "failed";
    case UMI_WORKBENCH_CONTEXT_SOURCE_STATE_STOPPED: return "stopped";
    default: return "unknown";
    }
}
