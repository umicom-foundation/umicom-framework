/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/channel_registry.c
 *
 * PURPOSE:
 *   Register named release channels and their promotion policies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A registry lets products add channels such as nightly or enterprise without scattering policy switches through Studio.
 */

#include "umicom/delivery/channel_registry.h"
#include "delivery_internal.h"
#include <string.h>
/*
 * Initialise channel registry from caller-provided values so later operations receive a
 * known state.
 */
void umi_channel_registry_init(UmiChannelRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry != NULL) (void)memset(registry, 0, sizeof(*registry));
}
/* Add channel registry only after its inputs and available capacity have been checked. */
UmiStatus umi_channel_registry_add(UmiChannelRegistry *registry,
                                   const char *channel_id,
                                   UmiReleaseChannelPolicy policy)
{
    UmiChannelEntry *entry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || channel_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= 16U) return UMI_STATUS_CAPACITY_EXCEEDED;
    entry = &registry->entries[registry->count++];
    (void)memset(entry, 0, sizeof(*entry));
    entry->policy = policy;
    return umi_delivery_copy_text(entry->channel_id, sizeof(entry->channel_id), channel_id);
}
/*
 * Find channel registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiChannelEntry *umi_channel_registry_find(const UmiChannelRegistry *registry,
                                                 const char *channel_id)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || channel_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < registry->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(registry->entries[i].channel_id, channel_id) == 0) return &registry->entries[i];
    }
    return NULL;
}
