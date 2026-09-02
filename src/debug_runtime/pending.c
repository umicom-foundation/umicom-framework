/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/pending.c
 *
 * PURPOSE:
 *   Implement bounded DAP request correlation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/pending.h"

#include <stdio.h>
#include <string.h>

/*
 * Initialise debug runtime pending from caller-provided values so later operations receive
 * a known state.
 */
void umi_debug_runtime_pending_init(UmiDebugRuntimePendingTable *table)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (table != NULL) (void)memset(table, 0, sizeof(*table));
}

/*
 * Add debug runtime pending only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_debug_runtime_pending_add(
    UmiDebugRuntimePendingTable *table,
    uint64_t sequence,
    const char *command,
    const char *context)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (table == NULL || sequence == 0U ||
        command == NULL || command[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_DEBUG_RUNTIME_MAX_PENDING; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (table->items[index].active &&
            table->items[index].sequence == sequence) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_DEBUG_RUNTIME_MAX_PENDING; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!table->items[index].active) {
            UmiDebugRuntimePendingRequest *item = &table->items[index];

            (void)memset(item, 0, sizeof(*item));
            item->sequence = sequence;
            (void)snprintf(item->command, sizeof(item->command), "%s", command);
            (void)snprintf(
                item->context,
                sizeof(item->context),
                "%s",
                context != NULL ? context : "");
            item->created_sequence = ++table->sequence;
            item->active = 1;
            table->active_count += 1U;
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_CAPACITY_EXCEEDED;
}

/*
 * Find debug runtime pending while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_debug_runtime_pending_find(
    const UmiDebugRuntimePendingTable *table,
    uint64_t sequence,
    UmiDebugRuntimePendingRequest *out_request)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (table == NULL || sequence == 0U || out_request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_DEBUG_RUNTIME_MAX_PENDING; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (table->items[index].active &&
            table->items[index].sequence == sequence) {
            *out_request = table->items[index];
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the debug runtime pending take operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_pending_take(
    UmiDebugRuntimePendingTable *table,
    uint64_t sequence,
    UmiDebugRuntimePendingRequest *out_request)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (table == NULL || sequence == 0U || out_request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_DEBUG_RUNTIME_MAX_PENDING; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (table->items[index].active &&
            table->items[index].sequence == sequence) {
            *out_request = table->items[index];
            (void)memset(&table->items[index], 0, sizeof(table->items[index]));
            table->active_count -= 1U;
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}
