/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/replica_set.c
 *
 * PURPOSE:
 *   Maintain a bounded set of replica descriptors with stable identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/replica_set.h"
#include <string.h>

/* Registry initialisation is deterministic and allocation-free. */
void umi_data_replica_set_init(UmiDataReplicaSet *registry) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (registry != NULL) (void)memset(registry, 0, sizeof(*registry)); }

/* Duplicate identifiers are rejected to keep mapping semantics unambiguous. */
UmiStatus umi_data_replica_set_add(UmiDataReplicaSet *registry, const UmiDataReplicaDescriptor *item) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || item == NULL || item->replica_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < registry->count; ++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(registry->items[i].replica_id, item->replica_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_DATA_ENTERPRISE_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->items[registry->count++] = *item; registry->revision++;
    return UMI_STATUS_OK;
}

/* Lookup copies a snapshot so callers never borrow internal registry storage. */
UmiStatus umi_data_replica_set_find(const UmiDataReplicaSet *registry, const char *id, UmiDataReplicaDescriptor *out_item) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < registry->count; ++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(registry->items[i].replica_id, id) == 0) { *out_item = registry->items[i]; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}

/* Count exposes registry size without exposing mutable storage. */
size_t umi_data_replica_set_count(const UmiDataReplicaSet *registry) { return registry != NULL ? registry->count : 0U; }
