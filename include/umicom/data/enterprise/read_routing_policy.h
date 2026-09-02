/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/read_routing_policy.h
 *
 * PURPOSE:
 *   Select a healthy replica for read workloads according to consistency, priority and lag constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_READ_ROUTING_POLICY_H
#define UMICOM_DATA_ENTERPRISE_READ_ROUTING_POLICY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"
#include "umicom/data/enterprise/replica_set.h"
#include "umicom/data/enterprise/replication_lag.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data read routing policy data shared with callers of this public contract.
 */
typedef struct UmiDataReadRoutingPolicy { UmiDataConsistency consistency; uint64_t max_replica_lag_ms; bool prefer_replica; } UmiDataReadRoutingPolicy;
/* Initialise conservative session-consistent routing defaults. */ void umi_data_read_routing_policy_default(UmiDataReadRoutingPolicy *policy);
/* Select the best eligible replica index or return unavailable. */ UmiStatus umi_data_read_routing_policy_select(const UmiDataReadRoutingPolicy *policy,const UmiDataReplicaSet *replicas,const UmiDataReplicationLag *lags,size_t lag_count,size_t *out_index);

#ifdef __cplusplus
}
#endif
#endif
