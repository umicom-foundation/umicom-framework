/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/cet1_ratio.c
 *
 * PURPOSE:
 *   Calculate the Common Equity Tier 1 capital ratio.
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

#include "umicom/finance/prudential/cet1_ratio.h"

/*
 * Provide the pru cet1 ratio calculate operation used by this module and its client
 * applications.
 */
UmiStatus umi_pru_cet1_ratio_calculate(UmiPrudentialCet1Ratio *result,double numerator,double denominator) { UmiStatus s; double ratio; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(result==NULL)return UMI_STATUS_INVALID_ARGUMENT; s=umi_pru_ratio(numerator,denominator,&ratio); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; result->numerator=numerator; result->denominator=denominator; result->ratio=ratio; return UMI_STATUS_OK; }
