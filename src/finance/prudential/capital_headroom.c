/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/capital_headroom.c
 *
 * PURPOSE:
 *   Measure eligible capital headroom over the aggregate requirement.
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

#include "umicom/finance/prudential/capital_headroom.h"

UmiStatus umi_pru_capital_headroom_calculate(UmiPrudentialCapitalHeadroom *result,double available,double required) { if(result==NULL||!umi_pru_number_valid(available)||!umi_pru_number_valid(required)||available<0.0||required<0.0)return UMI_STATUS_INVALID_ARGUMENT; result->available=available; result->required=required; result->headroom=available-required; result->breached=result->headroom<0.0?1:0; return UMI_STATUS_OK; }
