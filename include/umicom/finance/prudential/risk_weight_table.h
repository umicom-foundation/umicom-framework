/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/risk_weight_table.h
 *
 * PURPOSE:
 *   Store and resolve ordered risk-weight rules by exposure-class identifier.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_RISK_WEIGHT_TABLE_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_RISK_WEIGHT_TABLE_H

#include "umicom/finance/prudential/types.h"
#include "umicom/finance/prudential/risk_weight_rule.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential risk weight table data shared with callers of this public
 * contract.
 */
typedef struct UmiPrudentialRiskWeightTable { UmiPrudentialRiskWeightRule rules[UMI_PRU_MAX_ITEMS]; size_t count; } UmiPrudentialRiskWeightTable;
/* Add or reject a duplicate exposure-class risk-weight rule. */
UmiStatus umi_pru_risk_weight_table_add(UmiPrudentialRiskWeightTable *table, const UmiPrudentialRiskWeightRule *rule);
/* Resolve a risk weight by exact exposure-class identifier. */
UmiStatus umi_pru_risk_weight_table_find(const UmiPrudentialRiskWeightTable *table, const char *exposure_class_id, double *out_weight);

#ifdef __cplusplus
}
#endif

#endif
