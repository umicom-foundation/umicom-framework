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
#include "umicom/finance/currency.h"

/* The earlier initializer and validator are retained for engineering review.
 * Shared bounded currency validation replaces the fixed-field strlen, and
 * staged identifiers keep a failing trade reference from changing the owner. */
#if 0
#include <string.h>
/* Initialize cashflow. */ UmiStatus umi_cashflow_init(UmiCashflow *c,const char *id,const char *trade,UmiMoney amount,UmiFinancialDirection dir,UmiFinancialDate date){UmiStatus st;if(c==NULL||dir==UMI_FINANCIAL_DIRECTION_NONE||strlen(amount.currency.code)!=3U||!umi_financial_date_is_valid(date))return UMI_STATUS_INVALID_ARGUMENT;st=umi_financial_id_assign(&c->cashflow_id,id);if(st!=UMI_STATUS_OK)return st;st=umi_trade_id_set(&c->trade_id,trade);if(st!=UMI_STATUS_OK)return st;c->amount=amount;c->direction=dir;c->payment_date=date;return UMI_STATUS_OK;}
/* Validate cashflow. */ bool umi_cashflow_is_valid(const UmiCashflow *c){return c!=NULL&&umi_financial_id_is_valid(&c->cashflow_id)&&umi_trade_id_is_valid(&c->trade_id)&&strlen(c->amount.currency.code)==3U&&c->direction!=UMI_FINANCIAL_DIRECTION_NONE&&umi_financial_date_is_valid(c->payment_date);}
#endif

static bool CashflowTermsValid(UmiMoney amount, UmiFinancialDirection direction, UmiFinancialDate date)
{
    return (direction == UMI_FINANCIAL_DIRECTION_PAY || direction == UMI_FINANCIAL_DIRECTION_RECEIVE) &&
        amount.scale <= 9U && umi_currency_valid(&amount.currency) && umi_financial_date_is_valid(date);
}

UmiStatus umi_cashflow_init(UmiCashflow *cashflow, const char *id, const char *trade,
    UmiMoney amount, UmiFinancialDirection direction, UmiFinancialDate date)
{
    if (cashflow == NULL || !CashflowTermsValid(amount, direction, date))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiCashflow candidate = {.amount = amount, .direction = direction, .payment_date = date};
    UmiStatus status = umi_financial_id_assign(&candidate.cashflow_id, id);
    if (status == UMI_STATUS_OK) status = umi_trade_id_set(&candidate.trade_id, trade);
    if (status != UMI_STATUS_OK) return status;
    if (!umi_financial_id_is_valid(&candidate.cashflow_id) || !umi_trade_id_is_valid(&candidate.trade_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    *cashflow = candidate;
    return UMI_STATUS_OK;
}

bool umi_cashflow_is_valid(const UmiCashflow *cashflow)
{
    return cashflow != NULL && umi_financial_id_is_valid(&cashflow->cashflow_id) &&
        umi_trade_id_is_valid(&cashflow->trade_id) &&
        CashflowTermsValid(cashflow->amount, cashflow->direction, cashflow->payment_date);
}
