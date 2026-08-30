/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/total_capital_ratio.c
 *
 * PURPOSE:
 *   Calculate the total regulatory capital ratio.
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

#include "umicom/finance/prudential/total_capital_ratio.h"

UmiStatus umi_pru_total_capital_ratio_calculate(UmiPrudentialTotalCapitalRatio *result,double numerator,double denominator) { UmiStatus s; double ratio; if(result==NULL)return UMI_STATUS_INVALID_ARGUMENT; s=umi_pru_ratio(numerator,denominator,&ratio); if(s!=UMI_STATUS_OK)return s; result->numerator=numerator; result->denominator=denominator; result->ratio=ratio; return UMI_STATUS_OK; }
