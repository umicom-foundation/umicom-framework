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

UmiStatus umi_pru_hqla_pool_add(UmiPrudentialHqlaPool *pool,const UmiPrudentialLiquidityAsset *asset) { if(pool==NULL||asset==NULL)return UMI_STATUS_INVALID_ARGUMENT; if(pool->count>=UMI_PRU_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; pool->assets[pool->count++]=*asset; return UMI_STATUS_OK; }
double umi_pru_hqla_pool_eligible_total(const UmiPrudentialHqlaPool *pool) { size_t i; double total=0.0; if(pool==NULL)return 0.0; for(i=0U;i<pool->count;++i)total+=pool->assets[i].eligible_value; return total; }
