/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/valuation_worker.c
 *
 * PURPOSE:
 *   Track bounded capacity and heartbeat state for a valuation worker.
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

#include "umicom/finance/enterprise/valuation_worker.h"

#include <string.h>
UmiStatus umi_enterprise_valuation_worker_init(UmiEnterpriseValuationWorker *w,const char *id,size_t cap){ UmiStatus s; if(w==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT; memset(w,0,sizeof *w); s=umi_quant_copy_text(w->worker_id,sizeof w->worker_id,id); if(s!=UMI_STATUS_OK)return s; w->capacity=cap;w->enabled=1;return UMI_STATUS_OK; }
UmiStatus umi_enterprise_valuation_worker_acquire(UmiEnterpriseValuationWorker *w){ if(w==NULL||w->enabled==0)return UMI_STATUS_INVALID_ARGUMENT; if(w->active_tasks>=w->capacity)return UMI_STATUS_BUSY; w->active_tasks++;return UMI_STATUS_OK; }
UmiStatus umi_enterprise_valuation_worker_release(UmiEnterpriseValuationWorker *w){ if(w==NULL||w->active_tasks==0U)return UMI_STATUS_INVALID_STATE; w->active_tasks--;return UMI_STATUS_OK; }
