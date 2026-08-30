/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/capital_tier.c
 *
 * PURPOSE:
 *   Classify eligible capital by CET1, AT1 or Tier 2 and retain its amount.
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

#include <string.h>
UmiStatus umi_pru_capital_tier_init(UmiPrudentialCapitalTier *record, const char *resource_id, UmiCapitalTierKind tier, double amount, UmiCurrency currency) { UmiStatus s; if(record==NULL||resource_id==NULL||!umi_pru_number_valid(amount)||amount<0.0||tier<UMI_PRU_CAPITAL_CET1||tier>UMI_PRU_CAPITAL_TIER2) return UMI_STATUS_INVALID_ARGUMENT; memset(record,0,sizeof *record); s=umi_pru_copy_text(record->resource_id,sizeof record->resource_id,resource_id); if(s!=UMI_STATUS_OK)return s; record->tier=tier; record->amount=amount; record->currency=currency; return UMI_STATUS_OK; }
