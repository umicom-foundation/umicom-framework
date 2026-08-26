/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/funding_concentration.c
 *
 * PURPOSE:
 *   Measure largest-source concentration within total funding.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/funding_concentration.h"

UmiStatus umi_pru_funding_concentration_calculate(UmiPrudentialFundingConcentration *result,double numerator,double denominator) { UmiStatus s; double ratio; if(result==NULL)return UMI_STATUS_INVALID_ARGUMENT; s=umi_pru_ratio(numerator,denominator,&ratio); if(s!=UMI_STATUS_OK)return s; result->numerator=numerator; result->denominator=denominator; result->ratio=ratio; return UMI_STATUS_OK; }
