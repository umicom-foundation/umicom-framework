/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/data_platform_snapshot.c
 *
 * PURPOSE:
 *   Project point-in-time Data Server enterprise state for CLI, Studio, Desk and observability frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/data_platform_snapshot.h"
#include <string.h>

/* Snapshotting copies scalar evidence only and never exposes backend connection/object ownership. */ UmiStatus umi_data_data_platform_snapshot_build(UmiDataPlatformSnapshot *snapshot,uint64_t schema_fingerprint,const UmiDataEntityRegistry *entities,const UmiDataConnectionPool *pool,const UmiDataReplicaSet *replicas,const UmiDataOperationQueue *queue,UmiDataEnterpriseState health_state,uint64_t revision){if(snapshot==NULL||entities==NULL||pool==NULL||replicas==NULL||queue==NULL||schema_fingerprint==0U)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(snapshot,0,sizeof(*snapshot));snapshot->schema_fingerprint=schema_fingerprint;snapshot->entity_count=entities->count;snapshot->pool_total=pool->count;snapshot->pool_leased=pool->leased_count;snapshot->replica_count=replicas->count;snapshot->queued_operations=queue->count;snapshot->health_state=health_state;snapshot->revision=revision;return UMI_STATUS_OK;}
