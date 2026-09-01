/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/channel_registry.h
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

#ifndef INCLUDE_UMICOM_DELIVERY_CHANNEL_REGISTRY_H
#define INCLUDE_UMICOM_DELIVERY_CHANNEL_REGISTRY_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"
#include "umicom/delivery/release_channel.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiChannelEntry {
    char channel_id[UMI_DELIVERY_ID_CAPACITY];
    UmiReleaseChannelPolicy policy;
} UmiChannelEntry;
typedef struct UmiChannelRegistry {
    UmiChannelEntry entries[16U];
    size_t count;
} UmiChannelRegistry;
void umi_channel_registry_init(UmiChannelRegistry *registry);
UmiStatus umi_channel_registry_add(UmiChannelRegistry *registry,
                                   const char *channel_id,
                                   UmiReleaseChannelPolicy policy);
const UmiChannelEntry *umi_channel_registry_find(const UmiChannelRegistry *registry,
                                                 const char *channel_id);

#ifdef __cplusplus
}
#endif

#endif
