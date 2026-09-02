/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/capital_headroom.h
 *
 * PURPOSE:
 *   Measure eligible capital headroom over the aggregate requirement.
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

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CAPITAL_HEADROOM_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CAPITAL_HEADROOM_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the prudential capital headroom data shared with callers of this public
 * contract.
 */
typedef struct UmiPrudentialCapitalHeadroom { double available; double required; double headroom; int breached; } UmiPrudentialCapitalHeadroom;
/* Calculate prudential headroom and identify a deficit without hiding negative values. */
UmiStatus umi_pru_capital_headroom_calculate(UmiPrudentialCapitalHeadroom *result, double available, double required);

#ifdef __cplusplus
}
#endif

#endif
