/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/capital_projection.h
 *
 * PURPOSE:
 *   Project capital after earnings, distributions, issuance and deductions.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CAPITAL_PROJECTION_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CAPITAL_PROJECTION_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential capital projection data shared with callers of this public
 * contract.
 */
typedef struct UmiPrudentialCapitalProjection { double opening_capital; double earnings; double distributions; double issuance; double deductions; double closing_capital; } UmiPrudentialCapitalProjection;
/* Project closing capital from transparent additive sources and uses. */
UmiStatus umi_pru_capital_projection_calculate(UmiPrudentialCapitalProjection *result, double opening_capital, double earnings, double distributions, double issuance, double deductions);

#ifdef __cplusplus
}
#endif

#endif
