/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/distributed_worker.c
 *
 * PURPOSE:
 *   Track heartbeat, capacity and lease evidence for distributed valuation workers.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/distributed_worker.h"

#include <string.h>
/*
 * Initialise enterprise distributed worker from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_enterprise_distributed_worker_init(UmiEnterpriseDistributedWorker *w,const char *id,size_t cap){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(w==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;memset(w,0,sizeof *w);w->capacity=cap;return umi_quant_copy_text(w->worker_id,sizeof w->worker_id,id);}
/*
 * Provide the enterprise distributed worker heartbeat operation used by this module and
 * its client applications.
 */
UmiStatus umi_enterprise_distributed_worker_heartbeat(UmiEnterpriseDistributedWorker *w,int64_t hb,int64_t lease){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(w==NULL||hb<0||lease<=hb||hb<w->heartbeat_ms)return UMI_STATUS_INVALID_ARGUMENT;w->heartbeat_ms=hb;w->lease_until_ms=lease;return UMI_STATUS_OK;}
/*
 * Provide the enterprise distributed worker available operation used by this module and
 * its client applications.
 */
int umi_enterprise_distributed_worker_available(const UmiEnterpriseDistributedWorker *w,int64_t now){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(w==NULL||now<0)return 0;return(now<=w->lease_until_ms&&w->active_jobs<w->capacity)?1:0;}
