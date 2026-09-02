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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/stress_result_store.h"

/*
 * Add pru stress result store only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_pru_stress_result_store_add(UmiPrudentialStressResultStore *store,const UmiPrudentialStressResult *result) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(store==NULL||result==NULL)return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(store->count>=UMI_PRU_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; store->results[store->count++]=*result; return UMI_STATUS_OK; }
/*
 * Return the number of records represented by pru stress result store failed without
 * changing their state.
 */
size_t umi_pru_stress_result_store_failed_count(const UmiPrudentialStressResultStore *store) { size_t i,n=0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(store==NULL)return 0U; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<store->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(store->results[i].passed==0)++n; return n; }
