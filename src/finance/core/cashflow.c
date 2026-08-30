/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/cashflow.c
 *
 * PURPOSE:
 *   Implement dated directional cashflows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/cashflow.h"

#include <string.h>
/* Initialize cashflow. */ UmiStatus umi_cashflow_init(UmiCashflow *c,const char *id,const char *trade,UmiMoney amount,UmiFinancialDirection dir,UmiFinancialDate date){UmiStatus st;if(c==NULL||dir==UMI_FINANCIAL_DIRECTION_NONE||strlen(amount.currency.code)!=3U||!umi_financial_date_is_valid(date))return UMI_STATUS_INVALID_ARGUMENT;st=umi_financial_id_assign(&c->cashflow_id,id);if(st!=UMI_STATUS_OK)return st;st=umi_trade_id_set(&c->trade_id,trade);if(st!=UMI_STATUS_OK)return st;c->amount=amount;c->direction=dir;c->payment_date=date;return UMI_STATUS_OK;}
/* Validate cashflow. */ bool umi_cashflow_is_valid(const UmiCashflow *c){return c!=NULL&&umi_financial_id_is_valid(&c->cashflow_id)&&umi_trade_id_is_valid(&c->trade_id)&&strlen(c->amount.currency.code)==3U&&c->direction!=UMI_FINANCIAL_DIRECTION_NONE&&umi_financial_date_is_valid(c->payment_date);}
