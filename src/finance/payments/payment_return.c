/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_return.c
 *
 * PURPOSE:
 *   Implement represent full or partial payment returns with bounded reason codes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_return.h"
#include <string.h>
UmiStatus umi_payments_payment_return_init(UmiPaymentsPaymentReturn *value,
    const char *id,
    const char *original_payment_id,
    const char *reason_code,
    int64_t amount_minor) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_payments_id_assign(&value->original_payment_id,original_payment_id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_financial_core_copy(value->reason_code,sizeof value->reason_code,reason_code);
    if(rc!=UMI_STATUS_OK)return rc;
    value->amount_minor=amount_minor;
    return umi_payments_payment_return_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_payments_payment_return_valid(const UmiPaymentsPaymentReturn *value) {
    return value!=NULL && (value->reason_code[0]!='\0' && value->amount_minor>0);
}
