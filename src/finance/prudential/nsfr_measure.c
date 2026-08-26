/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/nsfr_measure.c
 *
 * PURPOSE:
 *   Calculate the net stable funding ratio.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/nsfr_measure.h"

UmiStatus umi_pru_nsfr_measure_calculate(UmiPrudentialNsfrMeasure *result,double numerator,double denominator) { UmiStatus s; double ratio; if(result==NULL)return UMI_STATUS_INVALID_ARGUMENT; s=umi_pru_ratio(numerator,denominator,&ratio); if(s!=UMI_STATUS_OK)return s; result->numerator=numerator; result->denominator=denominator; result->ratio=ratio; return UMI_STATUS_OK; }
