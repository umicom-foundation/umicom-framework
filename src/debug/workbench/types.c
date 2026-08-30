/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/types.c
 *
 * PURPOSE:
 *   Define stable debugger-workbench identifiers, source locations, phases, commands and shared value types.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical Debug Service/DAP
 *   runtime state; Studio remains a thin frontend and owns no reusable debug
 *   semantics, adapter protocol, breakpoint engine or inspection engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/workbench/types.h"

#include <string.h>

UmiStatus umi_debug_workbench_copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

int umi_debug_workbench_id_valid(const char *identifier)
{
    return identifier != NULL && identifier[0] != '\0' && strlen(identifier) < UMI_DEBUG_WORKBENCH_ID_CAPACITY;
}

int umi_debug_workbench_position_compare(UmiDebugWorkbenchPosition left, UmiDebugWorkbenchPosition right)
{
    if (left.line < right.line) return -1;
    if (left.line > right.line) return 1;
    if (left.column < right.column) return -1;
    if (left.column > right.column) return 1;
    return 0;
}

UmiDebugWorkbenchRange umi_debug_workbench_range_normalize(UmiDebugWorkbenchRange range)
{
    if (umi_debug_workbench_position_compare(range.start, range.end) > 0) {
        UmiDebugWorkbenchPosition temporary = range.start;
        range.start = range.end;
        range.end = temporary;
    }
    return range;
}

UmiStatus umi_debug_workbench_location_init(UmiDebugWorkbenchLocation *location, const char *path, UmiDebugWorkbenchRange range)
{
    if (location == NULL || path == NULL || path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    memset(location, 0, sizeof *location);
    if (umi_debug_workbench_copy_text(location->path, sizeof location->path, path) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    location->range = umi_debug_workbench_range_normalize(range);
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_workbench_entry_init(UmiDebugWorkbenchEntry *entry, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range)
{
    if (entry == NULL || !umi_debug_workbench_id_valid(id) || label == NULL || label[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    memset(entry, 0, sizeof *entry);
    if (umi_debug_workbench_copy_text(entry->id, sizeof entry->id, id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_debug_workbench_copy_text(entry->label, sizeof entry->label, label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (detail != NULL && detail[0] != '\0' && umi_debug_workbench_copy_text(entry->detail, sizeof entry->detail, detail) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (path != NULL && path[0] != '\0') {
        if (umi_debug_workbench_location_init(&entry->location, path, range) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    }
    entry->revision = 1U;
    return UMI_STATUS_OK;
}

int umi_debug_workbench_entry_valid(const UmiDebugWorkbenchEntry *entry)
{
    return entry != NULL && umi_debug_workbench_id_valid(entry->id) && entry->label[0] != '\0' && entry->revision > 0U;
}

int umi_debug_workbench_session_transition_allowed(UmiDebugWorkbenchSessionPhase from, UmiDebugWorkbenchSessionPhase to)
{
    if (from == to) return 1;
    switch (from) {
        case UMI_DEBUG_WORKBENCH_SESSION_IDLE:
            return to == UMI_DEBUG_WORKBENCH_SESSION_INITIALIZING;
        case UMI_DEBUG_WORKBENCH_SESSION_INITIALIZING:
            return to == UMI_DEBUG_WORKBENCH_SESSION_RUNNING || to == UMI_DEBUG_WORKBENCH_SESSION_PAUSED || to == UMI_DEBUG_WORKBENCH_SESSION_FAILED || to == UMI_DEBUG_WORKBENCH_SESSION_TERMINATED;
        case UMI_DEBUG_WORKBENCH_SESSION_RUNNING:
            return to == UMI_DEBUG_WORKBENCH_SESSION_PAUSED || to == UMI_DEBUG_WORKBENCH_SESSION_STOPPED || to == UMI_DEBUG_WORKBENCH_SESSION_TERMINATED || to == UMI_DEBUG_WORKBENCH_SESSION_FAILED;
        case UMI_DEBUG_WORKBENCH_SESSION_PAUSED:
            return to == UMI_DEBUG_WORKBENCH_SESSION_RUNNING || to == UMI_DEBUG_WORKBENCH_SESSION_STOPPED || to == UMI_DEBUG_WORKBENCH_SESSION_TERMINATED || to == UMI_DEBUG_WORKBENCH_SESSION_FAILED;
        case UMI_DEBUG_WORKBENCH_SESSION_STOPPED:
            return to == UMI_DEBUG_WORKBENCH_SESSION_RUNNING || to == UMI_DEBUG_WORKBENCH_SESSION_TERMINATED;
        case UMI_DEBUG_WORKBENCH_SESSION_TERMINATED:
        case UMI_DEBUG_WORKBENCH_SESSION_FAILED:
            return to == UMI_DEBUG_WORKBENCH_SESSION_IDLE;
        default:
            return 0;
    }
}

uint64_t umi_debug_workbench_command_bit(UmiDebugWorkbenchCommand command)
{
    unsigned int shift;
    if (command < UMI_DEBUG_WORKBENCH_COMMAND_START || command >= UMI_DEBUG_WORKBENCH_COMMAND_COUNT) return 0U;
    shift = (unsigned int)command;
    return UINT64_C(1) << shift;
}
