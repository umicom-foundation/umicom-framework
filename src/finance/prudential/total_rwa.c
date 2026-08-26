/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/total_rwa.c
 *
 * PURPOSE:
 *   Aggregate credit, market and operational risk-weighted assets.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/total_rwa.h"

UmiStatus umi_pru_total_rwa_calculate(UmiPrudentialTotalRwa *result,double credit,double market,double operational) { if(result==NULL||!umi_pru_number_valid(credit)||!umi_pru_number_valid(market)||!umi_pru_number_valid(operational)||credit<0.0||market<0.0||operational<0.0)return UMI_STATUS_INVALID_ARGUMENT; result->credit_rwa=credit; result->market_rwa=market; result->operational_rwa=operational; result->total_rwa=credit+market+operational; return umi_pru_number_valid(result->total_rwa)?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE; }
