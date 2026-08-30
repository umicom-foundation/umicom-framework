/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/credit_rwa.c
 *
 * PURPOSE:
 *   Calculate credit risk-weighted assets from exposure and risk weight.
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

#include "umicom/finance/prudential/credit_rwa.h"

UmiStatus umi_pru_credit_rwa_calculate(UmiPrudentialCreditRwa *result,double exposure,double risk_weight) { double rwa; UmiStatus s; if(result==NULL)return UMI_STATUS_INVALID_ARGUMENT; s=umi_pru_weighted_amount(exposure,risk_weight,&rwa); if(s!=UMI_STATUS_OK)return s; result->exposure=exposure; result->risk_weight=risk_weight; result->rwa=rwa; return UMI_STATUS_OK; }
