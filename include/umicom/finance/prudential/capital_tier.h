/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/capital_tier.h
 *
 * PURPOSE:
 *   Classify eligible capital by CET1, AT1 or Tier 2 and retain its amount.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CAPITAL_TIER_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CAPITAL_TIER_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialCapitalTier { char resource_id[UMI_PRU_ID_CAPACITY]; UmiCapitalTierKind tier; double amount; UmiCurrency currency; } UmiPrudentialCapitalTier;
/* Initialise one eligible capital tier amount. */
UmiStatus umi_pru_capital_tier_init(UmiPrudentialCapitalTier *record, const char *resource_id, UmiCapitalTierKind tier, double amount, UmiCurrency currency);

#ifdef __cplusplus
}
#endif

#endif
