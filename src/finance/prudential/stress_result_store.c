/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/stress_result_store.c
 *
 * PURPOSE:
 *   Store bounded stress results and count failed control thresholds.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/stress_result_store.h"

UmiStatus umi_pru_stress_result_store_add(UmiPrudentialStressResultStore *store,const UmiPrudentialStressResult *result) { if(store==NULL||result==NULL)return UMI_STATUS_INVALID_ARGUMENT; if(store->count>=UMI_PRU_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; store->results[store->count++]=*result; return UMI_STATUS_OK; }
size_t umi_pru_stress_result_store_failed_count(const UmiPrudentialStressResultStore *store) { size_t i,n=0U; if(store==NULL)return 0U; for(i=0U;i<store->count;++i)if(store->results[i].passed==0)++n; return n; }
