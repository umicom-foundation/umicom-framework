/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/prudential/credit_rwa.h
 *
 * PURPOSE:
 *   Calculate credit risk-weighted assets from exposure and risk weight.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CREDIT_RWA_H
#define INCLUDE_UMICOM_FINANCE_PRUDENTIAL_CREDIT_RWA_H

#include "umicom/finance/prudential/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPrudentialCreditRwa { double exposure; double risk_weight; double rwa; } UmiPrudentialCreditRwa;
/* Calculate risk-weighted assets from exposure and risk weight. */
UmiStatus umi_pru_credit_rwa_calculate(UmiPrudentialCreditRwa *result, double exposure, double risk_weight);

#ifdef __cplusplus
}
#endif

#endif
