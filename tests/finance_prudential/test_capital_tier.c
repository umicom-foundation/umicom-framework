/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_capital_tier.c
 *
 * PURPOSE:
 *   Verify classify eligible capital by CET1, AT1 or Tier 2 and retain its amount.
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

#include "umicom/finance/prudential/capital_tier.h"

int main(void) { UmiPrudentialCapitalTier r; UmiCurrency c={"GBP"}; if(umi_pru_capital_tier_init(&r,"CET1",UMI_PRU_CAPITAL_CET1,100.0,c)!=UMI_STATUS_OK)return 1; if(r.amount!=100.0||r.tier!=UMI_PRU_CAPITAL_CET1)return 2; return 0; }
