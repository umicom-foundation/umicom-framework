/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workflow_tools/test_decimal.c
 * PURPOSE:
 *   Check exact decimal input, output and overflow with release-active assertions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/decimal.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while(0)

static int Reject(const char *text, uint8_t scale, UmiStatus expected)
{
    UmiDecimal out = {77, 4};
    CHECK(UmiDecimalParse(text, strlen(text), scale, &out) == expected);
    CHECK(out.coefficient == 77 && out.scale == 4);
    return 0;
}
static int Run(const char *name)
{
    UmiDecimal value = {123, 2}, out = {77, 4};
    char text[UMI_DECIMAL_TEXT_MAX + 1U];
    if (strcmp(name,"parse-basic")==0) {
        CHECK(UmiDecimalParse("1250.75",7,2,&out)==UMI_STATUS_OK);
        CHECK(out.coefficient==125075 && out.scale==2);
        CHECK(UmiDecimalParse("12.5",4,2,&out)==UMI_STATUS_OK && out.coefficient==1250);
    } else if (strcmp(name,"parse-signs")==0) {
        CHECK(UmiDecimalParse(" \t-12.50\t",9,2,&out)==UMI_STATUS_OK && out.coefficient==-1250);
        CHECK(UmiDecimalParse("+0",2,9,&out)==UMI_STATUS_OK && out.coefficient==0);
        CHECK(UmiDecimalParse("-0.00",5,2,&out)==UMI_STATUS_OK && out.coefficient==0);
    } else if (strcmp(name,"parse-invalid")==0) {
        const char *bad[]={"","1,000.00","1e3",".5","1.","+-1","1.001","1 2","1\n","--2","NaN"};
        for(size_t i=0;i<sizeof bad/sizeof bad[0];++i)
            CHECK(Reject(bad[i],2,UMI_STATUS_INVALID_ARGUMENT)==0);
        CHECK(Reject("1.0",0,UMI_STATUS_INVALID_ARGUMENT)==0);
    } else if (strcmp(name,"parse-extremes")==0) {
        CHECK(UmiDecimalParse("9223372036854775807",19,0,&out)==UMI_STATUS_OK && out.coefficient==INT64_MAX);
        CHECK(UmiDecimalParse("-9223372036854775808",20,0,&out)==UMI_STATUS_OK && out.coefficient==INT64_MIN);
        CHECK(UmiDecimalParse("-92233720368547758.08",21,2,&out)==UMI_STATUS_OK && out.coefficient==INT64_MIN);
    } else if (strcmp(name,"parse-overflow")==0) {
        CHECK(Reject("9223372036854775808",0,UMI_STATUS_CAPACITY_EXCEEDED)==0);
        CHECK(Reject("-9223372036854775809",0,UMI_STATUS_CAPACITY_EXCEEDED)==0);
        CHECK(Reject("92233720368547759",2,UMI_STATUS_CAPACITY_EXCEEDED)==0);
    } else if (strcmp(name,"parse-counted")==0) {
        const char zero[]={'1','\0','2'};
        CHECK(UmiDecimalParse(zero,3,2,&out)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(out.coefficient==77 && out.scale==4);
        CHECK(UmiDecimalParse("1",UMI_DECIMAL_TEXT_MAX+1U,2,&out)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiDecimalParse(NULL,1,2,&out)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDecimalParse("1",1,10,&out)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDecimalParse("1",1,2,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name,"format-basic")==0) {
        CHECK(UmiDecimalFormat(value,text,sizeof text)==UMI_STATUS_OK && strcmp(text,"1.23")==0);
        CHECK(UmiDecimalFormat((UmiDecimal){-1,3},text,sizeof text)==UMI_STATUS_OK && strcmp(text,"-0.001")==0);
    } else if (strcmp(name,"format-capacity")==0) {
        strcpy(text,"unchanged");
        CHECK(UmiDecimalFormat(value,text,4)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(text,"unchanged")==0);
        CHECK(UmiDecimalFormat((UmiDecimal){1,10},text,sizeof text)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(text,"unchanged")==0);
    } else if (strcmp(name,"roundtrip")==0) {
        int64_t values[]={INT64_MIN,INT64_MAX,-1,0,1,-987654321,123456789};
        for(uint8_t scale=0;scale<=9;++scale) {
            for(size_t i=0;i<sizeof values/sizeof values[0];++i) {
                value=(UmiDecimal){values[i],scale};
                CHECK(UmiDecimalFormat(value,text,sizeof text)==UMI_STATUS_OK);
                CHECK(UmiDecimalParse(text,strlen(text),scale,&out)==UMI_STATUS_OK);
                CHECK(out.coefficient==value.coefficient && out.scale==value.scale);
            }
        }
    } else if (strcmp(name,"rescale")==0) {
        CHECK(umi_decimal_rescale(value,4,&out)==UMI_STATUS_OK && out.coefficient==12300 && out.scale==4);
        CHECK(umi_decimal_rescale((UmiDecimal){-129,2},1,&out)==UMI_STATUS_OK && out.coefficient==-12);
        CHECK(umi_decimal_rescale((UmiDecimal){INT64_MIN,9},0,&out)==UMI_STATUS_OK);
    } else if (strcmp(name,"rescale-overflow")==0) {
        CHECK(umi_decimal_rescale((UmiDecimal){INT64_MAX,0},1,&out)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(out.coefficient==77 && out.scale==4);
        CHECK(umi_decimal_rescale((UmiDecimal){INT64_MIN,0},1,&out)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(out.coefficient==77 && out.scale==4);
    } else if (strcmp(name,"rescale-invalid")==0) {
        CHECK(umi_decimal_rescale((UmiDecimal){1,255},0,&out)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(out.coefficient==77 && out.scale==4);
        CHECK(umi_decimal_rescale(value,10,&out)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_decimal_rescale(value,1,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    return 0;
}
int main(int argc,char **argv) {return argc==2?Run(argv[1]):2;}
