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

void umi_diagnostic_hub_init(UmiDiagnosticHub *hub)
{
    if (hub == NULL) {
        return;
    }
    (void)memset(hub, 0, sizeof(*hub));
}

UmiStatus umi_diagnostic_hub_add(UmiDiagnosticHub *hub,
                                 UmiDiagnosticSink sink,
                                 void *user_data)
{
    size_t index;
    if (hub == NULL || sink == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < hub->count; ++index) {
        if (hub->entries[index].sink == sink &&
            hub->entries[index].user_data == user_data) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (hub->count >= UMI_DIAGNOSTIC_HUB_MAX_SINKS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    hub->entries[hub->count].sink = sink;
    hub->entries[hub->count].user_data = user_data;
    ++hub->count;
    return UMI_STATUS_OK;
}

UmiStatus umi_diagnostic_hub_remove(UmiDiagnosticHub *hub,
                                    UmiDiagnosticSink sink,
                                    void *user_data)
{
    size_t index;
    if (hub == NULL || sink == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < hub->count; ++index) {
        if (hub->entries[index].sink == sink &&
            hub->entries[index].user_data == user_data) {
            size_t move_index;
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

size_t umi_diagnostic_hub_count(const UmiDiagnosticHub *hub)
{
    return hub != NULL ? hub->count : 0U;
}

void umi_diagnostic_hub_emit(const UmiDiagnostic *diagnostic, void *user_data)
{
    UmiDiagnosticHub *hub = (UmiDiagnosticHub *)user_data;
    size_t index;
    if (hub == NULL || diagnostic == NULL) {
        return;
    }
    for (index = 0U; index < hub->count; ++index) {
        UmiDiagnosticSink sink = hub->entries[index].sink;
        if (sink != NULL) {
            sink(diagnostic, hub->entries[index].user_data);
        }
    }
}
