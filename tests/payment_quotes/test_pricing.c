/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/payment_quotes/test_pricing.c
 * PURPOSE: Exercise arbitrary fees, checked charges and owned quote evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *test=argv[1];UmiPaymentQuoteRequest r=Request();UmiPaymentQuote *quote=NULL;UmiPaymentQuoteSnapshot s;
    int64_t amount=71;
    if(strcmp(test,"arbitrary-fee")==0){
        OK(umi_payments_payment_fee_rule_calculate(&r.rule,12345,UMI_MONEY_HALF_EVEN,&amount));CHECK(amount==41);
        OK(umi_payments_payment_fee_rule_calculate(&r.rule,12345,UMI_MONEY_TOWARD_ZERO,&amount));CHECK(amount==40);
        OK(umi_payments_payment_fee_rule_calculate(&r.rule,0,UMI_MONEY_HALF_EVEN,&amount));CHECK(amount==10);
        CHECK(umi_payments_payment_fee_rule_fee_for_10000_minor(&r.rule)==35);
    }else if(strcmp(test,"cap-before-add")==0){
        OK(umi_payments_payment_fee_rule_init(&r.rule,"large",INT64_MAX-10,10000,INT64_MAX));
        OK(umi_payments_payment_fee_rule_calculate(&r.rule,INT64_MAX,UMI_MONEY_AWAY_FROM_ZERO,&amount));CHECK(amount==INT64_MAX);
        CHECK(umi_payments_payment_fee_rule_fee_for_10000_minor(&r.rule)==INT64_MAX);
        OK(umi_payments_payment_fee_rule_init(&r.rule,"zero",0,10000,0));
        OK(umi_payments_payment_fee_rule_calculate(&r.rule,INT64_MAX,UMI_MONEY_HALF_EVEN,&amount));CHECK(amount==0);
    }else if(strcmp(test,"rule-atomic")==0){
        UmiPaymentsPaymentFeeRule old=r.rule;
        CHECK(umi_payments_payment_fee_rule_init(&r.rule,"invalid",10,25,9)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&r.rule,&old,sizeof(old))==0);
        OK(umi_payments_payment_fee_rule_init(&r.rule,r.rule.id.value,15,20,100));CHECK(strcmp(r.rule.id.value,"rule-1")==0);
        CHECK(umi_payments_payment_fee_rule_calculate(&r.rule,-1,UMI_MONEY_HALF_EVEN,&amount)==UMI_STATUS_INVALID_ARGUMENT&&amount==71);
        r.rule.id.value[0]='\0';CHECK(!umi_payments_payment_fee_rule_valid(&r.rule));
        CHECK(umi_payments_payment_fee_rule_fee_for_10000_minor(&r.rule)==0);
    }else if(strcmp(test,"charge-overflow")==0){
        UmiPaymentsPaymentCharge charge;OK(umi_payments_payment_charge_init(&charge,"charge","payment",INT64_MAX,0));
        OK(umi_payments_payment_charge_total_checked(&charge,&amount));CHECK(amount==INT64_MAX);
        charge.tax_minor=1;amount=71;
        CHECK(!umi_payments_payment_charge_valid(&charge));CHECK(umi_payments_payment_charge_total_checked(&charge,&amount)==UMI_STATUS_CAPACITY_EXCEEDED&&amount==71);
        CHECK(umi_payments_payment_charge_total_minor(&charge)==0);
    }else if(strcmp(test,"charge-atomic")==0){
        UmiPaymentsPaymentCharge charge;OK(umi_payments_payment_charge_init(&charge,"charge","payment",25,5));
        UmiPaymentsPaymentCharge old=charge;
        CHECK(umi_payments_payment_charge_init(&charge,"x","y",INT64_MAX,1)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&charge,&old,sizeof(old))==0);
        CHECK(umi_payments_payment_charge_init(&charge,"x","y",-1,1)==UMI_STATUS_INVALID_ARGUMENT&&memcmp(&charge,&old,sizeof(old))==0);
        OK(umi_payments_payment_charge_init(&charge,charge.id.value,charge.payment_id.value,40,2));
        CHECK(strcmp(charge.id.value,"charge")==0&&strcmp(charge.payment_id.value,"payment")==0&&umi_payments_payment_charge_total_minor(&charge)==42);
    }else if(strcmp(test,"bad-identities")==0){
        UmiPaymentsPaymentCharge charge={0};CHECK(!umi_payments_payment_charge_valid(&charge));
        CHECK(umi_payments_payment_charge_total_checked(&charge,&amount)==UMI_STATUS_INVALID_ARGUMENT&&amount==71);
        memset(r.rule.id.value,'x',sizeof(r.rule.id.value));CHECK(!umi_payments_payment_fee_rule_valid(&r.rule));
        CHECK(umi_payments_payment_fee_rule_calculate(&r.rule,1,UMI_MONEY_HALF_EVEN,&amount)==UMI_STATUS_INVALID_ARGUMENT&&amount==71);
    }else if(strcmp(test,"quote")==0){
        OK(UmiPaymentQuoteCreate(&r,&quote));OK(UmiPaymentQuoteRead(quote,&s));
        CHECK(s.fee.minor_units==35&&s.tax.minor_units==7&&s.charges.minor_units==42&&s.totalDebit.minor_units==10042);
        CHECK(s.request.principal.minor_units==10000&&s.totalDebit.scale==2&&strcmp(s.totalDebit.currency.code,"GBP")==0);
    }else if(strcmp(test,"cap-tax")==0){
        r.rule.maximum_fee_minor=20;r.taxBasisPoints=2500;
        OK(UmiPaymentQuoteCreate(&r,&quote));OK(UmiPaymentQuoteRead(quote,&s));
        CHECK(s.fee.minor_units==20&&s.tax.minor_units==5&&s.totalDebit.minor_units==10025);
    }else if(strcmp(test,"separate-rounding")==0){
        r.principal.minor_units=5;r.rule.fixed_fee_minor=0;r.rule.variable_fee_bps=5000;r.taxBasisPoints=5000;
        r.feeRounding=UMI_MONEY_HALF_AWAY;r.taxRounding=UMI_MONEY_TOWARD_ZERO;
        OK(UmiPaymentQuoteCreate(&r,&quote));OK(UmiPaymentQuoteRead(quote,&s));
        CHECK(s.fee.minor_units==3&&s.tax.minor_units==1&&s.totalDebit.minor_units==9);
    }else if(strcmp(test,"quote-overflow")==0){
        r.principal.minor_units=INT64_MAX;r.rule.fixed_fee_minor=1;r.rule.variable_fee_bps=0;r.taxBasisPoints=0;
        CHECK(UmiPaymentQuoteCreate(&r,&quote)==UMI_STATUS_CAPACITY_EXCEEDED&&quote==NULL);
        r.principal.minor_units=1;r.rule.fixed_fee_minor=r.rule.maximum_fee_minor=INT64_MAX;r.taxBasisPoints=10000;
        CHECK(UmiPaymentQuoteCreate(&r,&quote)==UMI_STATUS_CAPACITY_EXCEEDED&&quote==NULL);
    }else if(strcmp(test,"quote-invalid")==0){
        CHECK(UmiPaymentQuoteCreate(NULL,&quote)==UMI_STATUS_INVALID_ARGUMENT&&quote==NULL);
        CHECK(UmiPaymentQuoteCreate(&r,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        r.principal.minor_units=0;CHECK(UmiPaymentQuoteCreate(&r,&quote)==UMI_STATUS_INVALID_ARGUMENT);
        r=Request();r.principal.scale=10;CHECK(UmiPaymentQuoteCreate(&r,&quote)==UMI_STATUS_INVALID_ARGUMENT);
        r=Request();r.taxBasisPoints=10001;CHECK(UmiPaymentQuoteCreate(&r,&quote)==UMI_STATUS_INVALID_ARGUMENT);
        r=Request();r.taxRounding=(UmiMoneyRounding)5;CHECK(UmiPaymentQuoteCreate(&r,&quote)==UMI_STATUS_INVALID_ARGUMENT);
        r=Request();memset(r.paymentId.value,'x',sizeof(r.paymentId.value));CHECK(UmiPaymentQuoteCreate(&r,&quote)==UMI_STATUS_INVALID_ARGUMENT);
        r=Request();r.principal.currency.code[0]='g';CHECK(UmiPaymentQuoteCreate(&r,&quote)==UMI_STATUS_INVALID_ARGUMENT);
    }else if(strcmp(test,"ownership")==0){
        OK(UmiPaymentQuoteCreate(&r,&quote));memset(&r,0,sizeof(r));OK(UmiPaymentQuoteRead(quote,&s));
        s.fee.minor_units=1;OK(UmiPaymentQuoteRead(quote,&s));CHECK(s.fee.minor_units==35&&s.request.principal.minor_units==10000);
        UmiPaymentQuoteSnapshot old=s;CHECK(UmiPaymentQuoteRead(NULL,&s)==UMI_STATUS_INVALID_ARGUMENT&&memcmp(&s,&old,sizeof(s))==0);
    }else if(strcmp(test,"text")==0){
        OK(UmiPaymentQuoteCreate(&r,&quote));size_t needed=0;OK(UmiPaymentQuoteDescribe(quote,NULL,0,&needed));
        char *text=malloc(needed);CHECK(text!=NULL);OK(UmiPaymentQuoteDescribe(quote,text,needed,NULL));
        CHECK(strstr(text,"Principal: 100.00")&&strstr(text,"Total charges: 0.42")&&strstr(text,"Principal plus charges: 100.42"));
        CHECK(strstr(text,"Tax on capped fee: 2000")&&strstr(text,"No payment was submitted"));
        char previous[8]="keep";CHECK(UmiPaymentQuoteDescribe(quote,previous,sizeof(previous),&needed)==UMI_STATUS_CAPACITY_EXCEEDED&&strcmp(previous,"keep")==0);
        free(text);
    }else if(strcmp(test,"csv")==0){
        strcpy(r.quoteId.value,"=scenario");OK(UmiPaymentQuoteCreate(&r,&quote));UmiCsvDocument *csv=NULL;OK(UmiPaymentQuoteExportCsv(quote,&csv));
        UmiPaymentQuoteDestroy(quote);quote=NULL;memset(&r,0,sizeof(r));
        const char *data=UmiCsvDocumentData(csv);CHECK(strstr(data,"'=scenario")&&strstr(data,"\"35\",\"7\",\"42\",\"10042\"")&&strstr(data,"tax_bps_on_fee"));
        CHECK(strstr(data,"no tariff/tax determination"));UmiCsvDocumentDestroy(csv);
    }else if(strcmp(test,"zero-fee")==0){
        r.principal.minor_units=INT64_MAX;r.rule.fixed_fee_minor=0;r.rule.maximum_fee_minor=0;r.taxBasisPoints=10000;
        OK(UmiPaymentQuoteCreate(&r,&quote));OK(UmiPaymentQuoteRead(quote,&s));CHECK(s.charges.minor_units==0&&s.totalDebit.minor_units==INT64_MAX);
    }else return 2;
    UmiPaymentQuoteDestroy(quote);return 0;
}
