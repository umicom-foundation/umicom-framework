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

/*
 * Provide the debug workbench copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_workbench_copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Check that debug workbench id satisfies its contract before another service relies on
 * it.
 */
int umi_debug_workbench_id_valid(const char *identifier)
{
    return identifier != NULL && identifier[0] != '\0' && strlen(identifier) < UMI_DEBUG_WORKBENCH_ID_CAPACITY;
}

/*
 * Provide the debug workbench position compare operation used by this module and its
 * client applications.
 */
int umi_debug_workbench_position_compare(UmiDebugWorkbenchPosition left, UmiDebugWorkbenchPosition right)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (left.line < right.line) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left.line > right.line) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left.column < right.column) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left.column > right.column) return 1;
    return 0;
}

/*
 * Provide the debug workbench range normalize operation used by this module and its client
 * applications.
 */
UmiDebugWorkbenchRange umi_debug_workbench_range_normalize(UmiDebugWorkbenchRange range)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_debug_workbench_position_compare(range.start, range.end) > 0) {
        UmiDebugWorkbenchPosition temporary = range.start;
        range.start = range.end;
        range.end = temporary;
    }
    return range;
}

/*
 * Initialise debug workbench location from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_workbench_location_init(UmiDebugWorkbenchLocation *location, const char *path, UmiDebugWorkbenchRange range)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (location == NULL || path == NULL || path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    memset(location, 0, sizeof *location);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_debug_workbench_copy_text(location->path, sizeof location->path, path) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    location->range = umi_debug_workbench_range_normalize(range);
    return UMI_STATUS_OK;
}

/*
 * Initialise debug workbench entry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_debug_workbench_entry_init(UmiDebugWorkbenchEntry *entry, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL || !umi_debug_workbench_id_valid(id) || label == NULL || label[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    memset(entry, 0, sizeof *entry);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_debug_workbench_copy_text(entry->id, sizeof entry->id, id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_debug_workbench_copy_text(entry->label, sizeof entry->label, label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (detail != NULL && detail[0] != '\0' && umi_debug_workbench_copy_text(entry->detail, sizeof entry->detail, detail) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path != NULL && path[0] != '\0') {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (umi_debug_workbench_location_init(&entry->location, path, range) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    }
    entry->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Check that debug workbench entry satisfies its contract before another service relies on
 * it.
 */
int umi_debug_workbench_entry_valid(const UmiDebugWorkbenchEntry *entry)
{
    return entry != NULL && umi_debug_workbench_id_valid(entry->id) && entry->label[0] != '\0' && entry->revision > 0U;
}

/*
 * Provide the debug workbench session transition allowed operation used by this module and
 * its client applications.
 */
int umi_debug_workbench_session_transition_allowed(UmiDebugWorkbenchSessionPhase from, UmiDebugWorkbenchSessionPhase to)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (from == to) return 1;
    /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Provide the debug workbench command bit operation used by this module and its client
 * applications.
 */
uint64_t umi_debug_workbench_command_bit(UmiDebugWorkbenchCommand command)
{
    unsigned int shift;
    /* Use the shared build helper when it is available from the parent composition. */
    if (command < UMI_DEBUG_WORKBENCH_COMMAND_START || command >= UMI_DEBUG_WORKBENCH_COMMAND_COUNT) return 0U;
    shift = (unsigned int)command;
    return UINT64_C(1) << shift;
}
