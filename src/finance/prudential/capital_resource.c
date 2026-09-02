/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/capital_resource.c
 *
 * PURPOSE:
 *   Represent an eligible capital resource with adjustments and deductions.
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

#include "umicom/finance/prudential/capital_resource.h"

#include <string.h>
/*
 * Initialise pru capital resource from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_pru_capital_resource_init(UmiPrudentialCapitalResource *record,const char *resource_id,UmiCapitalTierKind tier,double gross_amount,double deductions,UmiCurrency currency) { UmiStatus s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(record==NULL||resource_id==NULL||!umi_pru_number_valid(gross_amount)||!umi_pru_number_valid(deductions)||gross_amount<0.0||deductions<0.0||deductions>gross_amount)return UMI_STATUS_INVALID_ARGUMENT; memset(record,0,sizeof *record); s=umi_pru_copy_text(record->resource_id,sizeof record->resource_id,resource_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; record->tier=tier; record->gross_amount=gross_amount; record->deductions=deductions; record->eligible_amount=gross_amount-deductions; record->currency=currency; return UMI_STATUS_OK; }
