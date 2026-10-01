/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/payment_quotes/test_rate.c
 * PURPOSE: Check exact rate rounding, signed boundaries and failure atomicity.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *test=argv[1];int64_t out=731;
    if(strcmp(test,"rounding")==0){
        struct {int64_t amount;uint32_t rate;int64_t expected[4];} cases[]={
            {5,5000,{2,3,2,3}},{7,5000,{3,4,4,4}},{-5,5000,{-2,-3,-2,-3}},{-7,5000,{-3,-4,-4,-4}},
            {4999,1,{0,0,0,1}},{5000,1,{0,1,0,1}},{5001,1,{0,1,1,1}},{15000,1,{1,2,2,2}},
            {-1,1,{0,0,0,-1}},{0,10000,{0,0,0,0}},{123,10000,{123,123,123,123}}};
        for(size_t i=0;i<sizeof(cases)/sizeof(cases[0]);++i)for(int mode=0;mode<4;++mode){
            OK(UmiMinorApplyBasisPoints(cases[i].amount,cases[i].rate,(UmiMoneyRounding)mode,&out));CHECK(out==cases[i].expected[mode]);
        }
    }else if(strcmp(test,"limits")==0){
        OK(UmiMinorApplyBasisPoints(INT64_MAX,10000,UMI_MONEY_AWAY_FROM_ZERO,&out));CHECK(out==INT64_MAX);
        OK(UmiMinorApplyBasisPoints(INT64_MIN,10000,UMI_MONEY_HALF_EVEN,&out));CHECK(out==INT64_MIN);
        OK(UmiMinorApplyBasisPoints(INT64_MAX,5000,UMI_MONEY_HALF_EVEN,&out));CHECK(out==INT64_C(4611686018427387904));
        OK(UmiMinorApplyBasisPoints(INT64_MIN,5000,UMI_MONEY_HALF_EVEN,&out));CHECK(out==-INT64_C(4611686018427387904));
        OK(UmiMinorApplyBasisPoints(INT64_MAX,1,UMI_MONEY_TOWARD_ZERO,&out));CHECK(out==INT64_C(922337203685477));
        OK(UmiMinorApplyBasisPoints(INT64_MAX,1,UMI_MONEY_HALF_EVEN,&out));CHECK(out==INT64_C(922337203685478));
        OK(UmiMinorApplyBasisPoints(INT64_MIN,0,UMI_MONEY_AWAY_FROM_ZERO,&out));CHECK(out==0);
    }else if(strcmp(test,"invalid")==0){
        CHECK(UmiMinorApplyBasisPoints(1,10001,UMI_MONEY_HALF_EVEN,&out)==UMI_STATUS_INVALID_ARGUMENT&&out==731);
        CHECK(UmiMinorApplyBasisPoints(1,1,(UmiMoneyRounding)-1,&out)==UMI_STATUS_INVALID_ARGUMENT&&out==731);
        CHECK(UmiMinorApplyBasisPoints(1,1,(UmiMoneyRounding)4,&out)==UMI_STATUS_INVALID_ARGUMENT&&out==731);
        CHECK(UmiMinorApplyBasisPoints(1,1,UMI_MONEY_HALF_EVEN,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    }else if(strcmp(test,"money")==0){
        UmiMoney money={.minor_units=-5,.scale=9,.currency={{'J','P','Y','\0'}}};
        OK(UmiMoneyApplyBasisPoints(&money,5000,UMI_MONEY_HALF_AWAY,&money));
        CHECK(money.minor_units==-3&&money.scale==9&&strcmp(money.currency.code,"JPY")==0);
        UmiMoney before=money;money.scale=10;
        CHECK(UmiMoneyApplyBasisPoints(&money,1,UMI_MONEY_HALF_EVEN,&before)==UMI_STATUS_INVALID_ARGUMENT&&before.minor_units==-3&&before.scale==9);
        money.scale=2;money.currency.code[3]='x';
        CHECK(UmiMoneyApplyBasisPoints(&money,1,UMI_MONEY_HALF_EVEN,&before)==UMI_STATUS_INVALID_ARGUMENT&&before.minor_units==-3);
        CHECK(UmiMoneyApplyBasisPoints(NULL,1,UMI_MONEY_HALF_EVEN,&before)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiMoneyApplyBasisPoints(&before,1,UMI_MONEY_HALF_EVEN,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    }else return 2;
    return 0;
}
