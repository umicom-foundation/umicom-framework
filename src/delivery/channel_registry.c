/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/channel_registry.c
 *
 * PURPOSE:
 *   Register named release channels and their promotion policies.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A registry lets products add channels such as nightly or enterprise without scattering policy switches through Studio.
 */

#include "umicom/delivery/channel_registry.h"
#include "delivery_internal.h"
#include <string.h>
void umi_channel_registry_init(UmiChannelRegistry *registry)
{
    if (registry != NULL) (void)memset(registry, 0, sizeof(*registry));
}
UmiStatus umi_channel_registry_add(UmiChannelRegistry *registry,
                                   const char *channel_id,
                                   UmiReleaseChannelPolicy policy)
{
    UmiChannelEntry *entry;
    if (registry == NULL || channel_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (registry->count >= 16U) return UMI_STATUS_CAPACITY_EXCEEDED;
    entry = &registry->entries[registry->count++];
    (void)memset(entry, 0, sizeof(*entry));
    entry->policy = policy;
    return umi_delivery_copy_text(entry->channel_id, sizeof(entry->channel_id), channel_id);
}
const UmiChannelEntry *umi_channel_registry_find(const UmiChannelRegistry *registry,
                                                 const char *channel_id)
{
    size_t i;
    if (registry == NULL || channel_id == NULL) return NULL;
    for (i = 0U; i < registry->count; ++i) {
        if (strcmp(registry->entries[i].channel_id, channel_id) == 0) return &registry->entries[i];
    }
    return NULL;
}
