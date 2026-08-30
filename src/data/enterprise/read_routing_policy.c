/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/read_routing_policy.c
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
#include "umicom/data/enterprise/read_routing_policy.h"
#include <string.h>

/* Default routing permits low-lag replicas while preserving a primary fallback. */ void umi_data_read_routing_policy_default(UmiDataReadRoutingPolicy *policy){if(policy==NULL)return;(void)memset(policy,0,sizeof(*policy));policy->consistency=UMI_DATA_CONSISTENCY_SESSION;policy->max_replica_lag_ms=250U;policy->prefer_replica=true;}
static uint64_t lag_for(const UmiDataReplicationLag *lags,size_t count,const char *id,bool *known){size_t i;*known=false;for(i=0;i<count;++i)if(strcmp(lags[i].replica_id,id)==0){*known=true;return lags[i].lag_ms;}return 0U;}
/* Strong consistency routes to a healthy primary; weaker modes choose the lowest-priority eligible endpoint. */ UmiStatus umi_data_read_routing_policy_select(const UmiDataReadRoutingPolicy *policy,const UmiDataReplicaSet *replicas,const UmiDataReplicationLag *lags,size_t lag_count,size_t *out_index){size_t i,best=(size_t)-1;uint32_t best_priority=UINT32_MAX;if(policy==NULL||replicas==NULL||out_index==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<replicas->count;++i){bool known=false;uint64_t lag=lag_for(lags,lag_count,replicas->items[i].replica_id,&known);if(!replicas->items[i].healthy)continue;if(policy->consistency==UMI_DATA_CONSISTENCY_STRONG&&!replicas->items[i].primary)continue;if(!replicas->items[i].primary&&(!known||lag>policy->max_replica_lag_ms))continue;if(policy->prefer_replica&&replicas->items[i].primary&&best!=(size_t)-1)continue;if(replicas->items[i].priority<best_priority){best=i;best_priority=replicas->items[i].priority;}}if(best==(size_t)-1)return UMI_STATUS_UNAVAILABLE;*out_index=best;return UMI_STATUS_OK;}
