/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/buying_power_model.c
 *
 * PURPOSE:
 *   Compute reusable brokerage buying power from cash, equity and margin requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/buying_power_model.h"

UmiStatus umi_trading_buying_power_model_compute(int64_t cash_minor,int64_t equity_minor,int64_t reserved_margin_minor,int64_t *out_buying_power_minor){if(out_buying_power_minor==NULL||cash_minor<0||equity_minor<0||reserved_margin_minor<0)return UMI_STATUS_INVALID_ARGUMENT;int64_t gross=0;if(umi_trading_core_add_i64(cash_minor,equity_minor,&gross)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;*out_buying_power_minor=gross>reserved_margin_minor?gross-reserved_margin_minor:0;return UMI_STATUS_OK;}
