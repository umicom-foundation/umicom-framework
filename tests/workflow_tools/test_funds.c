/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workflow_tools/test_funds.c
 * PURPOSE:
 *   Check banking and treasury reviews without booking any transaction.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance_ui/funds_review.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while(0)

static UmiMoney Money(int64_t value) {UmiMoney m={value,2,{{'G','B','P','\0'}}};return m;}
static int Run(const char *name)
{
    UmiMoney a=Money(100000),b=Money(12500),c=Money(20000),d=Money(10000);
    UmiFinanceAccountReview account = {0};
    UmiFinanceCashReview cash = {0};
    if(strcmp(name,"account")==0) {
        CHECK(UmiFinanceReviewAccount(&a,&b,&c,&account)==UMI_STATUS_OK);
        CHECK(account.available.minor_units==87500 && account.remaining_after_payment.minor_units==67500);
        CHECK(a.minor_units==100000 && b.minor_units==12500 && c.minor_units==20000);
    } else if(strcmp(name,"shortfall")==0) {
        a.minor_units=100;
        CHECK(UmiFinanceReviewAccount(&a,&b,&c,&account)==UMI_STATUS_OK);
        CHECK(account.remaining_after_payment.minor_units==-32400);
        CHECK(UmiFinanceReviewCash(&a,&b,&c,&d,&cash)==UMI_STATUS_OK);
        CHECK(cash.projected.minor_units==-7400 && cash.buffer_headroom.minor_units==-17400);
    } else if(strcmp(name,"cash")==0) {
        CHECK(UmiFinanceReviewCash(&a,&b,&c,&d,&cash)==UMI_STATUS_OK);
        CHECK(cash.net_flow.minor_units==-7500 && cash.projected.minor_units==92500);
        CHECK(cash.buffer_headroom.minor_units==82500);
    } else if(strcmp(name,"currency")==0) {
        strcpy(b.currency.code,"USD");account.available.minor_units=79;
        cash.projected.minor_units=81;
        CHECK(UmiFinanceReviewAccount(&a,&b,&c,&account)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(account.available.minor_units==79);
        CHECK(UmiFinanceReviewCash(&a,&b,&c,&d,&cash)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(cash.projected.minor_units==81);
    } else if(strcmp(name,"scale")==0) {
        b.scale=3; account.available.minor_units=79;
        CHECK(UmiFinanceReviewAccount(&a,&b,&c,&account)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(account.available.minor_units==79);
        a.scale=10;b.scale=10;c.scale=10;d.scale=10;
        CHECK(UmiFinanceReviewCash(&a,&b,&c,&d,&cash)==UMI_STATUS_INVALID_ARGUMENT);
    } else if(strcmp(name,"negative-input")==0) {
        b.minor_units=-1;
        CHECK(UmiFinanceReviewAccount(&a,&b,&c,&account)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFinanceReviewCash(&a,&b,&c,&d,&cash)==UMI_STATUS_INVALID_ARGUMENT);
        b.minor_units=0;c.minor_units=-1;
        CHECK(UmiFinanceReviewAccount(&a,&b,&c,&account)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFinanceReviewCash(&a,&b,&c,&d,&cash)==UMI_STATUS_INVALID_ARGUMENT);
        c.minor_units=0;d.minor_units=-1;
        CHECK(UmiFinanceReviewCash(&a,&b,&c,&d,&cash)==UMI_STATUS_INVALID_ARGUMENT);
    } else if(strcmp(name,"overflow")==0) {
        account.available.minor_units=79;cash.projected.minor_units=81;
        a.minor_units=INT64_MIN;b.minor_units=1;
        CHECK(UmiFinanceReviewAccount(&a,&b,&c,&account)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(account.available.minor_units==79);
        a.minor_units=INT64_MAX;b.minor_units=1;c.minor_units=0;
        CHECK(UmiFinanceReviewCash(&a,&b,&c,&d,&cash)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(cash.projected.minor_units==81);
        a.minor_units=INT64_MIN;b.minor_units=0;c.minor_units=1;
        CHECK(UmiFinanceReviewCash(&a,&b,&c,&d,&cash)==UMI_STATUS_CAPACITY_EXCEEDED);
    } else if(strcmp(name,"invalid")==0) {
        CHECK(UmiFinanceReviewAccount(NULL,&b,&c,&account)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFinanceReviewAccount(&a,&b,&c,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFinanceReviewCash(NULL,&b,&c,&d,&cash)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiFinanceReviewCash(&a,&b,&c,NULL,&cash)==UMI_STATUS_INVALID_ARGUMENT);
        memset(a.currency.code,'X',4);
        CHECK(UmiFinanceReviewAccount(&a,&b,&c,&account)==UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    return 0;
}
int main(int argc,char **argv){return argc==2?Run(argv[1]):2;}
