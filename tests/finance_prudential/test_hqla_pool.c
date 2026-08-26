/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_hqla_pool.c
 *
 * PURPOSE:
 *   Verify aggregate HQLA assets into an available liquidity stock.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/hqla_pool.h"

int main(void) { UmiPrudentialHqlaPool p={0}; UmiPrudentialLiquidityAsset a; UmiCurrency c={"GBP"}; if(umi_pru_liquidity_asset_init(&a,"A",UMI_PRU_HQLA_LEVEL1,100.0,0.1,c)!=UMI_STATUS_OK)return 1; if(umi_pru_hqla_pool_add(&p,&a)!=UMI_STATUS_OK)return 2; return umi_pru_hqla_pool_eligible_total(&p)==90.0?0:3; }
