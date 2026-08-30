/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/leverage_ratio.c
 *
 * PURPOSE:
 *   Calculate the prudential leverage ratio against total exposure measure.
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

#include "umicom/finance/prudential/leverage_ratio.h"

UmiStatus umi_pru_leverage_ratio_calculate(UmiPrudentialLeverageRatio *result,double numerator,double denominator) { UmiStatus s; double ratio; if(result==NULL)return UMI_STATUS_INVALID_ARGUMENT; s=umi_pru_ratio(numerator,denominator,&ratio); if(s!=UMI_STATUS_OK)return s; result->numerator=numerator; result->denominator=denominator; result->ratio=ratio; return UMI_STATUS_OK; }
