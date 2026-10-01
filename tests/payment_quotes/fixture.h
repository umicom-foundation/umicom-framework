/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/payment_quotes/fixture.h
 * PURPOSE: Share explicit quote assumptions for regression cases.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PAYMENT_QUOTES_FIXTURE_H
#define UMICOM_PAYMENT_QUOTES_FIXTURE_H
#include "umicom/finance/payments/payment_quote.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);exit(1); } } while(0)
#define OK(x) CHECK((x)==UMI_STATUS_OK)
static inline UmiPaymentQuoteRequest Request(void)
{
    UmiPaymentQuoteRequest r={0};
    OK(umi_payments_id_assign(&r.quoteId,"quote-1"));OK(umi_payments_id_assign(&r.paymentId,"payment-1"));
    memcpy(r.principal.currency.code,"GBP",4);r.principal.scale=2;r.principal.minor_units=10000;
    OK(umi_payments_payment_fee_rule_init(&r.rule,"rule-1",10,25,1000));
    r.feeRounding=UMI_MONEY_HALF_EVEN;r.taxBasisPoints=2000;r.taxRounding=UMI_MONEY_HALF_EVEN;
    return r;
}
#endif
