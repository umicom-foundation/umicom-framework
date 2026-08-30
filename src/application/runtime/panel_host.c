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

void umi_application_panel_host_init(UmiApplicationPanelHost *host)
{
    if (host == NULL) return;
    memset(host, 0, sizeof(*host));
    host->structure_size = sizeof(*host);
}

const UmiApplicationPanelHostEntry *umi_application_panel_host_find(
    const UmiApplicationPanelHost *host,
    const char *panel_id)
{
    size_t index;
    if (host == NULL || panel_id == NULL) return NULL;
    for (index = 0U; index < host->entry_count; ++index) {
        if (strcmp(host->entries[index].panel_id, panel_id) == 0)
            return &host->entries[index];
    }
    return NULL;
}

UmiStatus umi_application_panel_host_register(
    UmiApplicationPanelHost *host,
    const UmiApplicationExperienceDefinition *experience,
    const char *panel_id,
    void *frontend_handle)
{
    UmiApplicationPanelHostEntry *entry;
    if (host == NULL || experience == NULL || panel_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_application_experience_panel_find(experience, panel_id) == NULL)
        return UMI_STATUS_NOT_FOUND;
    if (umi_application_panel_host_find(host, panel_id) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    if (host->entry_count >= UMI_APPLICATION_RUNTIME_MAX_PANELS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    entry = &host->entries[host->entry_count++];
    entry->panel_id = panel_id;
    entry->frontend_handle = frontend_handle;
    entry->available = 1;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_panel_host_unregister(
    UmiApplicationPanelHost *host,
    const char *panel_id)
{
    size_t index;
    if (host == NULL || panel_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < host->entry_count; ++index) {
        if (strcmp(host->entries[index].panel_id, panel_id) == 0) {
            for (; index + 1U < host->entry_count; ++index)
                host->entries[index] = host->entries[index + 1U];
            host->entry_count -= 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
