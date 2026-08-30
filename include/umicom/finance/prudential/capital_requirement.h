/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/capital_requirement.h
 *
 * PURPOSE:
 *   Aggregate minimum capital and additional buffer requirements.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CAPITAL_REQUIREMENT_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CAPITAL_REQUIREMENT_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialCapitalRequirement { double minimum_requirement; double conservation_buffer; double countercyclical_buffer; double systemic_buffer; double total_requirement; } UmiPrudentialCapitalRequirement;
/* Aggregate minimum capital and named additional buffer requirements. */
UmiStatus umi_pru_capital_requirement_calculate(UmiPrudentialCapitalRequirement *result, double minimum_requirement, double conservation_buffer, double countercyclical_buffer, double systemic_buffer);

#ifdef __cplusplus
}
#endif

#endif
