/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/lcr_measure.c
 *
 * PURPOSE:
 *   Calculate the liquidity coverage ratio from HQLA and net stressed outflows.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/lcr_measure.h"

UmiStatus umi_pru_lcr_measure_calculate(UmiPrudentialLcrMeasure *result,double numerator,double denominator) { UmiStatus s; double ratio; if(result==NULL)return UMI_STATUS_INVALID_ARGUMENT; s=umi_pru_ratio(numerator,denominator,&ratio); if(s!=UMI_STATUS_OK)return s; result->numerator=numerator; result->denominator=denominator; result->ratio=ratio; return UMI_STATUS_OK; }
