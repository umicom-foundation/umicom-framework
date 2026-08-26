/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/capital_projection.c
 *
 * PURPOSE:
 *   Project capital after earnings, distributions, issuance and deductions.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/capital_projection.h"

UmiStatus umi_pru_capital_projection_calculate(UmiPrudentialCapitalProjection *result,double opening,double earnings,double distributions,double issuance,double deductions) { double closing; if(result==NULL||opening<0.0||distributions<0.0||issuance<0.0||deductions<0.0||!umi_pru_number_valid(earnings))return UMI_STATUS_INVALID_ARGUMENT; closing=opening+earnings-distributions+issuance-deductions; if(!umi_pru_number_valid(closing))return UMI_STATUS_INVALID_STATE; result->opening_capital=opening; result->earnings=earnings; result->distributions=distributions; result->issuance=issuance; result->deductions=deductions; result->closing_capital=closing; return UMI_STATUS_OK; }
