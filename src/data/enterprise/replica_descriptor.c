/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/replica_descriptor.c
 *
 * PURPOSE:
 *   Describe a Data Server replica endpoint, health and role without embedding network transport ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/replica_descriptor.h"
#include <string.h>

/* Initialisation centralises bounded text handling and defaults. */
UmiStatus umi_data_replica_descriptor_init(UmiDataReplicaDescriptor *item, const char *replica_id, const char *endpoint, uint32_t priority, bool primary) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(item, 0, sizeof(*item));
    UmiStatus s=umi_data_enterprise_copy_text(item->replica_id,sizeof(item->replica_id),replica_id);if(s!=UMI_STATUS_OK)return s;s=umi_data_enterprise_copy_text(item->endpoint,sizeof(item->endpoint),endpoint);if(s!=UMI_STATUS_OK)return s;item->priority=priority;item->primary=primary;item->healthy=true;item->writable=primary;
    return umi_data_replica_descriptor_validate(item);
}

/* Validation prevents malformed metadata from leaking into later query/migration stages. */
UmiStatus umi_data_replica_descriptor_validate(const UmiDataReplicaDescriptor *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(item->replica_id[0] != '\0' && item->endpoint[0] != '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
