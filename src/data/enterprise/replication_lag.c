/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/replication_lag.c
 *
 * PURPOSE:
 *   Classify replica freshness from observed sequence/timestamp lag without binding to a replication protocol.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/replication_lag.h"
#include <string.h>

/* Lag state is deterministic and can combine backend-specific measurements later. */ UmiStatus umi_data_replication_lag_compute(UmiDataReplicationLag *lag,const char *replica_id,uint64_t lag_ms,uint64_t sequence_gap,uint64_t warning_ms,uint64_t failure_ms){UmiStatus s;if(lag==NULL||replica_id==NULL||warning_ms>failure_ms)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(lag,0,sizeof(*lag));s=umi_data_enterprise_copy_text(lag->replica_id,sizeof(lag->replica_id),replica_id);if(s!=UMI_STATUS_OK)return s;lag->lag_ms=lag_ms;lag->sequence_gap=sequence_gap;lag->state=lag_ms>=failure_ms?UMI_DATA_ENTERPRISE_FAILED:(lag_ms>=warning_ms?UMI_DATA_ENTERPRISE_DEGRADED:UMI_DATA_ENTERPRISE_READY);return UMI_STATUS_OK;}
