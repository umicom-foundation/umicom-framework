/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/types.c
 *
 * PURPOSE:
 *   Implement shared bounded helpers for the banking financial platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/types.h"
#include <limits.h>
#include <string.h>
/*
 * Copy banking id into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_banking_id_assign(UmiFinancialId *id,const char *text) {
    return umi_financial_id_assign(id,text);
}
/*
 * Provide the banking currency from code operation used by this module and its client
 * applications.
 */
UmiStatus umi_banking_currency_from_code(const char *code,UmiCurrency *out_currency) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(code==NULL || out_currency==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(code[0]=='\0'||code[1]=='\0'||code[2]=='\0'||code[3]!='\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for(size_t i=0U;i<3U;++i) /* Preserve the original failure result so the caller can respond to the correct cause. */ if(code[i]<'A'||code[i]>'Z') return UMI_STATUS_INVALID_ARGUMENT;
    out_currency->code[0]=code[0]; out_currency->code[1]=code[1]; out_currency->code[2]=code[2]; out_currency->code[3]='\0';
    return UMI_STATUS_OK;
}
/*
 * Provide the banking currency equal operation used by this module and its client
 * applications.
 */
bool umi_banking_currency_equal(UmiCurrency left,UmiCurrency right) { return memcmp(left.code,right.code,4U)==0; }
/* Provide the banking abs i64 operation used by this module and its client applications. */
int64_t umi_banking_abs_i64(int64_t value) { /* Apply this branch only when its contract condition is satisfied. */ if(value==INT64_MIN) return INT64_MAX; return value<0 ? -value : value; }
