/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/panel_host.c
 *
 * PURPOSE:
 *   Implement bounded panel-host registration keyed by canonical experience panel identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/panel_host.h"

#include <string.h>

/*
 * Initialise application panel host from caller-provided values so later operations
 * receive a known state.
 */
void umi_application_panel_host_init(UmiApplicationPanelHost *host)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == NULL) return;
    memset(host, 0, sizeof(*host));
    host->structure_size = sizeof(*host);
}

/*
 * Find application panel host while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiApplicationPanelHostEntry *umi_application_panel_host_find(
    const UmiApplicationPanelHost *host,
    const char *panel_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == NULL || panel_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < host->entry_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(host->entries[index].panel_id, panel_id) == 0)
            return &host->entries[index];
    }
    return NULL;
}

/*
 * Add application panel host only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_application_panel_host_register(
    UmiApplicationPanelHost *host,
    const UmiApplicationExperienceDefinition *experience,
    const char *panel_id,
    void *frontend_handle)
{
    UmiApplicationPanelHostEntry *entry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == NULL || experience == NULL || panel_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_application_experience_panel_find(experience, panel_id) == NULL)
        return UMI_STATUS_NOT_FOUND;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_application_panel_host_find(host, panel_id) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (host->entry_count >= UMI_APPLICATION_RUNTIME_MAX_PANELS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    entry = &host->entries[host->entry_count++];
    entry->panel_id = panel_id;
    entry->frontend_handle = frontend_handle;
    entry->available = 1;
    return UMI_STATUS_OK;
}

/*
 * Remove application panel host while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_application_panel_host_unregister(
    UmiApplicationPanelHost *host,
    const char *panel_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (host == NULL || panel_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < host->entry_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(host->entries[index].panel_id, panel_id) == 0) {
            /* Visit each bounded item once so every record receives the same rule. */
            for (; index + 1U < host->entry_count; ++index)
                host->entries[index] = host->entries[index + 1U];
            host->entry_count -= 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
