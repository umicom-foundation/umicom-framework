/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/hqla_pool.c
 *
 * PURPOSE:
 *   Aggregate HQLA assets into an available liquidity stock.
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

#include "umicom/finance/prudential/hqla_pool.h"

/* Add pru hqla pool only after its inputs and available capacity have been checked. */
UmiStatus umi_pru_hqla_pool_add(UmiPrudentialHqlaPool *pool,const UmiPrudentialLiquidityAsset *asset) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(pool==NULL||asset==NULL)return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(pool->count>=UMI_PRU_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; pool->assets[pool->count++]=*asset; return UMI_STATUS_OK; }
/*
 * Provide the pru hqla pool eligible total operation used by this module and its client
 * applications.
 */
double umi_pru_hqla_pool_eligible_total(const UmiPrudentialHqlaPool *pool) { size_t i; double total=0.0; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(pool==NULL)return 0.0; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<pool->count;++i)total+=pool->assets[i].eligible_value; return total; }
