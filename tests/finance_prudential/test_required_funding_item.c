/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_required_funding_item.c
 *
 * PURPOSE:
 *   Verify calculate required stable funding from balance and RSF factor.
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

#include "umicom/finance/prudential/required_funding_item.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialRequiredFundingItem r; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_required_funding_item_calculate(&r,"ITEM",100.0,0.6)!=UMI_STATUS_OK)return 1; return r.stressed_amount==60.0?0:2; }
