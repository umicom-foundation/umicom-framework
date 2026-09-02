/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/src/diagnostics/hub.c
 *
 * PURPOSE:
 *   Implement the hub behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/hub.c
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/hub.h"

#include <string.h>

/*
 * Initialise diagnostic hub from caller-provided values so later operations receive a
 * known state.
 */
void umi_diagnostic_hub_init(UmiDiagnosticHub *hub)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hub == NULL) {
        return;
    }
    (void)memset(hub, 0, sizeof(*hub));
}

/* Add diagnostic hub only after its inputs and available capacity have been checked. */
UmiStatus umi_diagnostic_hub_add(UmiDiagnosticHub *hub,
                                 UmiDiagnosticSink sink,
                                 void *user_data)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hub == NULL || sink == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < hub->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (hub->entries[index].sink == sink &&
            hub->entries[index].user_data == user_data) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (hub->count >= UMI_DIAGNOSTIC_HUB_MAX_SINKS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    hub->entries[hub->count].sink = sink;
    hub->entries[hub->count].user_data = user_data;
    ++hub->count;
    return UMI_STATUS_OK;
}

/*
 * Remove diagnostic hub while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_diagnostic_hub_remove(UmiDiagnosticHub *hub,
                                    UmiDiagnosticSink sink,
                                    void *user_data)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hub == NULL || sink == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < hub->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (hub->entries[index].sink == sink &&
            hub->entries[index].user_data == user_data) {
            size_t move_index;
            /* Visit each bounded item once so every record receives the same rule. */
            for (move_index = index + 1U; move_index < hub->count; ++move_index) {
                hub->entries[move_index - 1U] = hub->entries[move_index];
            }
            --hub->count;
            hub->entries[hub->count].sink = NULL;
            hub->entries[hub->count].user_data = NULL;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/* Return the number of records represented by diagnostic hub without changing their state. */
size_t umi_diagnostic_hub_count(const UmiDiagnosticHub *hub)
{
    return hub != NULL ? hub->count : 0U;
}

/*
 * Provide the diagnostic hub emit operation used by this module and its client
 * applications.
 */
void umi_diagnostic_hub_emit(const UmiDiagnostic *diagnostic, void *user_data)
{
    UmiDiagnosticHub *hub = (UmiDiagnosticHub *)user_data;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hub == NULL || diagnostic == NULL) {
        return;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < hub->count; ++index) {
        UmiDiagnosticSink sink = hub->entries[index].sink;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (sink != NULL) {
            sink(diagnostic, hub->entries[index].user_data);
        }
    }
}
