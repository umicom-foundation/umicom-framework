/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/types.c
 *
 * PURPOSE:
 *   Implement shared bounded helpers for the accounting financial platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/types.h"
#include <limits.h>
#include <string.h>
UmiStatus umi_accounting_id_assign(UmiFinancialId *id,const char *text) {
    return umi_financial_id_assign(id,text);
}
UmiStatus umi_accounting_currency_from_code(const char *code,UmiCurrency *out_currency) {
    if(code==NULL || out_currency==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if(code[0]=='\0'||code[1]=='\0'||code[2]=='\0'||code[3]!='\0') return UMI_STATUS_INVALID_ARGUMENT;
    for(size_t i=0U;i<3U;++i) if(code[i]<'A'||code[i]>'Z') return UMI_STATUS_INVALID_ARGUMENT;
    out_currency->code[0]=code[0]; out_currency->code[1]=code[1]; out_currency->code[2]=code[2]; out_currency->code[3]='\0';
    return UMI_STATUS_OK;
}
bool umi_accounting_currency_equal(UmiCurrency left,UmiCurrency right) { return memcmp(left.code,right.code,4U)==0; }
int64_t umi_accounting_abs_i64(int64_t value) { if(value==INT64_MIN) return INT64_MAX; return value<0 ? -value : value; }
