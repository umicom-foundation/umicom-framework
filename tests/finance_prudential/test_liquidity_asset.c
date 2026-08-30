/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_liquidity_asset.c
 *
 * PURPOSE:
 *   Verify represent an eligible liquidity asset and calculate post-haircut value.
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

#include "umicom/finance/prudential/liquidity_asset.h"

int main(void) { UmiPrudentialLiquidityAsset a; UmiCurrency c={"GBP"}; if(umi_pru_liquidity_asset_init(&a,"GILT",UMI_PRU_HQLA_LEVEL1,100.0,0.05,c)!=UMI_STATUS_OK)return 1; return a.eligible_value==95.0?0:2; }
