/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/replica_descriptor.h
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
#ifndef UMICOM_DATA_ENTERPRISE_REPLICA_DESCRIPTOR_H
#define UMICOM_DATA_ENTERPRISE_REPLICA_DESCRIPTOR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataReplicaDescriptor {
    char replica_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char endpoint[UMI_DATA_ENTERPRISE_PATH_CAPACITY];
    uint32_t priority;
    bool primary;
    bool healthy;
    bool writable;
} UmiDataReplicaDescriptor;

/* Initialise a validated replica descriptor descriptor. */
UmiStatus umi_data_replica_descriptor_init(UmiDataReplicaDescriptor *item, const char *replica_id, const char *endpoint, uint32_t priority, bool primary);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_replica_descriptor_validate(const UmiDataReplicaDescriptor *item);

#ifdef __cplusplus
}
#endif
#endif
