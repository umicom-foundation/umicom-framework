/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/data_platform_snapshot.h
 *
 * PURPOSE:
 *   Project point-in-time Data Server enterprise state for CLI, Studio, Desk and observability frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_DATA_PLATFORM_SNAPSHOT_H
#define UMICOM_DATA_ENTERPRISE_DATA_PLATFORM_SNAPSHOT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"
#include "umicom/data/enterprise/entity_registry.h"
#include "umicom/data/enterprise/connection_pool.h"
#include "umicom/data/enterprise/replica_set.h"
#include "umicom/data/enterprise/data_operation_queue.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data platform snapshot data shared with callers of this public contract.
 */
typedef struct UmiDataPlatformSnapshot { uint64_t schema_fingerprint; size_t entity_count; size_t pool_total; size_t pool_leased; size_t replica_count; size_t queued_operations; UmiDataEnterpriseState health_state; uint64_t revision; } UmiDataPlatformSnapshot;
/* Build a point-in-time snapshot from canonical enterprise-data models. */ UmiStatus umi_data_data_platform_snapshot_build(UmiDataPlatformSnapshot *snapshot,uint64_t schema_fingerprint,const UmiDataEntityRegistry *entities,const UmiDataConnectionPool *pool,const UmiDataReplicaSet *replicas,const UmiDataOperationQueue *queue,UmiDataEnterpriseState health_state,uint64_t revision);

#ifdef __cplusplus
}
#endif
#endif
