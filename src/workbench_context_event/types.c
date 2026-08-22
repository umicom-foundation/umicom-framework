/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_event/types.c
 *
 * PURPOSE:
 *   Implement bounded text, stable hashing and enum text conversion for workbench interaction events.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_event/types.h"

#include <string.h>

size_t umi_workbench_context_event_bounded_length(const char *text, size_t capacity)
{
    size_t length = 0U;
    if (text == NULL) return 0U;
    while (length < capacity && text[length] != '\0') ++length;
    return length;
}

bool umi_workbench_context_event_text_is_valid(const char *text, size_t capacity)
{
    if (text == NULL || capacity == 0U) return false;
    return umi_workbench_context_event_bounded_length(text, capacity) < capacity;
}

UmiStatus umi_workbench_context_event_copy_text(
    char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = umi_workbench_context_event_bounded_length(source, capacity);
    if (length >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (length > 0U) memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

uint64_t umi_workbench_context_event_hash_text(
    uint64_t hash, const char *text, size_t capacity)
{
    size_t length;
    size_t index;
    if (text == NULL) return hash;
    length = umi_workbench_context_event_bounded_length(text, capacity);
    for (index = 0U; index < length; ++index) {
        hash ^= (uint64_t)(unsigned char)text[index];
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

const char *umi_workbench_context_event_kind_text(UmiWorkbenchContextEventKind kind)
{
    switch (kind) {
    case UMI_WORKBENCH_CONTEXT_EVENT_PROJECT_SELECTION: return "project-selection";
    case UMI_WORKBENCH_CONTEXT_EVENT_EDITOR_OPEN: return "editor-open";
    case UMI_WORKBENCH_CONTEXT_EVENT_EDITOR_CARET: return "editor-caret";
    case UMI_WORKBENCH_CONTEXT_EVENT_EDITOR_SELECTION: return "editor-selection";
    case UMI_WORKBENCH_CONTEXT_EVENT_DIAGNOSTIC_SELECTION: return "diagnostic-selection";
    case UMI_WORKBENCH_CONTEXT_EVENT_SOURCE_CONTROL_SELECTION: return "source-control-selection";
    case UMI_WORKBENCH_CONTEXT_EVENT_TEST_SELECTION: return "test-selection";
    case UMI_WORKBENCH_CONTEXT_EVENT_DEBUG_LOCATION: return "debug-location";
    case UMI_WORKBENCH_CONTEXT_EVENT_TERMINAL_SESSION: return "terminal-session";
    case UMI_WORKBENCH_CONTEXT_EVENT_AI_SELECTION: return "ai-selection";
    case UMI_WORKBENCH_CONTEXT_EVENT_APPLICATION_SELECTION: return "application-selection";
    case UMI_WORKBENCH_CONTEXT_EVENT_INSTRUMENT_SELECTION: return "instrument-selection";
    case UMI_WORKBENCH_CONTEXT_EVENT_ACCOUNT_SELECTION: return "account-selection";
    case UMI_WORKBENCH_CONTEXT_EVENT_TRADE_SELECTION: return "trade-selection";
    case UMI_WORKBENCH_CONTEXT_EVENT_WORKSPACE_CHANGE: return "workspace-change";
    case UMI_WORKBENCH_CONTEXT_EVENT_MEDIA_SELECTION: return "media-selection";
    case UMI_WORKBENCH_CONTEXT_EVENT_GENERIC_SELECTION: return "generic-selection";
    case UMI_WORKBENCH_CONTEXT_EVENT_NONE: return "none";
    default: return "unknown";
    }
}

const char *umi_workbench_context_event_state_text(UmiWorkbenchContextEventState state)
{
    switch (state) {
    case UMI_WORKBENCH_CONTEXT_EVENT_CREATED: return "created";
    case UMI_WORKBENCH_CONTEXT_EVENT_ACCEPTED: return "accepted";
    case UMI_WORKBENCH_CONTEXT_EVENT_COALESCED: return "coalesced";
    case UMI_WORKBENCH_CONTEXT_EVENT_ROUTED: return "routed";
    case UMI_WORKBENCH_CONTEXT_EVENT_REJECTED: return "rejected";
    case UMI_WORKBENCH_CONTEXT_EVENT_DROPPED: return "dropped";
    default: return "unknown";
    }
}

const char *umi_workbench_context_event_priority_text(UmiWorkbenchContextEventPriority priority)
{
    switch (priority) {
    case UMI_WORKBENCH_CONTEXT_EVENT_PRIORITY_BACKGROUND: return "background";
    case UMI_WORKBENCH_CONTEXT_EVENT_PRIORITY_NORMAL: return "normal";
    case UMI_WORKBENCH_CONTEXT_EVENT_PRIORITY_INTERACTIVE: return "interactive";
    case UMI_WORKBENCH_CONTEXT_EVENT_PRIORITY_URGENT: return "urgent";
    default: return "unknown";
    }
}
