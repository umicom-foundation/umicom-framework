/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/replication_lag.h
 *
 * PURPOSE:
 *   Classify replica freshness from observed sequence/timestamp lag without binding to a replication protocol.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_REPLICATION_LAG_H
#define UMICOM_DATA_ENTERPRISE_REPLICATION_LAG_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataReplicationLag { char replica_id[UMI_DATA_ENTERPRISE_ID_CAPACITY]; uint64_t lag_ms; uint64_t sequence_gap; UmiDataEnterpriseState state; } UmiDataReplicationLag;
/* Compute replica lag state against caller-provided warning/failure thresholds. */ UmiStatus umi_data_replication_lag_compute(UmiDataReplicationLag *lag,const char *replica_id,uint64_t lag_ms,uint64_t sequence_gap,uint64_t warning_ms,uint64_t failure_ms);

#ifdef __cplusplus
}
#endif
#endif
