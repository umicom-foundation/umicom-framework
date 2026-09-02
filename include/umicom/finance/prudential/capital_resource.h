/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/capital_resource.h
 *
 * PURPOSE:
 *   Represent an eligible capital resource with adjustments and deductions.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CAPITAL_RESOURCE_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CAPITAL_RESOURCE_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential capital resource data shared with callers of this public
 * contract.
 */
typedef struct UmiPrudentialCapitalResource { char resource_id[UMI_PRU_ID_CAPACITY]; UmiCapitalTierKind tier; double gross_amount; double deductions; double eligible_amount; UmiCurrency currency; } UmiPrudentialCapitalResource;
/* Initialise a capital resource and derive its eligible amount after deductions. */
UmiStatus umi_pru_capital_resource_init(UmiPrudentialCapitalResource *record, const char *resource_id, UmiCapitalTierKind tier, double gross_amount, double deductions, UmiCurrency currency);

#ifdef __cplusplus
}
#endif

#endif
