/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_stable_funding_item.c
 *
 * PURPOSE:
 *   Verify calculate available stable funding from balance and ASF factor.
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

#include "umicom/finance/prudential/stable_funding_item.h"

int main(void) { UmiPrudentialStableFundingItem r; if(umi_pru_stable_funding_item_calculate(&r,"ITEM",100.0,0.6)!=UMI_STATUS_OK)return 1; return r.stressed_amount==60.0?0:2; }
