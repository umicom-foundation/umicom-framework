/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_source/types.c
 *
 * PURPOSE:
 *   Implement bounded text helpers, hashing and stable text conversion for live interaction source contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_source/types.h"

#include <string.h>

size_t umi_workbench_context_source_bounded_length(
    const char *text,
    size_t capacity)
{
    size_t length = 0U;
    if (text == NULL) return 0U;
    while (length < capacity && text[length] != '\0') ++length;
    return length;
}

bool umi_workbench_context_source_text_is_valid(
    const char *text,
    size_t capacity)
{
    if (text == NULL || capacity == 0U) return false;
    return umi_workbench_context_source_bounded_length(
        text, capacity) < capacity;
}

UmiStatus umi_workbench_context_source_copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = umi_workbench_context_source_bounded_length(
        source, capacity);
    if (length >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (length > 0U) memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

uint64_t umi_workbench_context_source_hash_text(
    uint64_t hash,
    const char *text,
    size_t capacity)
{
    size_t length;
    size_t index;
    if (text == NULL) return hash;
    length = umi_workbench_context_source_bounded_length(
        text, capacity);
    for (index = 0U; index < length; ++index) {
        hash ^= (uint64_t)(unsigned char)text[index];
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

const char *umi_workbench_context_source_kind_text(
    UmiWorkbenchContextSourceKind kind)
{
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

const char *umi_workbench_context_source_trigger_text(
    UmiWorkbenchContextSourceTrigger trigger)
{
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

const char *umi_workbench_context_source_state_text(
    UmiWorkbenchContextSourceState state)
{
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
