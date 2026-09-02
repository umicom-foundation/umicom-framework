/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/replica_set.h
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
#ifndef UMICOM_DATA_ENTERPRISE_REPLICA_SET_H
#define UMICOM_DATA_ENTERPRISE_REPLICA_SET_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"
#include "umicom/data/enterprise/replica_descriptor.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data replica set data shared with callers of this public contract.
 */
typedef struct UmiDataReplicaSet {
    UmiDataReplicaDescriptor items[UMI_DATA_ENTERPRISE_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiDataReplicaSet;

/* Reset the bounded replica set registry. */
void umi_data_replica_set_init(UmiDataReplicaSet *registry);
/* Add a unique item by stable identifier. */
UmiStatus umi_data_replica_set_add(UmiDataReplicaSet *registry, const UmiDataReplicaDescriptor *item);
/* Find a registry item by stable identifier. */
UmiStatus umi_data_replica_set_find(const UmiDataReplicaSet *registry, const char *id, UmiDataReplicaDescriptor *out_item);
/* Return the number of registered items. */
size_t umi_data_replica_set_count(const UmiDataReplicaSet *registry);

#ifdef __cplusplus
}
#endif
#endif
