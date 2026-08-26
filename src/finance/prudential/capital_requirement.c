/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/capital_requirement.c
 *
 * PURPOSE:
 *   Aggregate minimum capital and additional buffer requirements.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/capital_requirement.h"

UmiStatus umi_pru_capital_requirement_calculate(UmiPrudentialCapitalRequirement *result,double minimum,double conservation,double countercyclical,double systemic) { if(result==NULL||minimum<0.0||conservation<0.0||countercyclical<0.0||systemic<0.0)return UMI_STATUS_INVALID_ARGUMENT; result->minimum_requirement=minimum; result->conservation_buffer=conservation; result->countercyclical_buffer=countercyclical; result->systemic_buffer=systemic; result->total_requirement=minimum+conservation+countercyclical+systemic; return umi_pru_number_valid(result->total_requirement)?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE; }
