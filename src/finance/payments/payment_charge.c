/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_charge.c
 *
 * PURPOSE:
 *   Implement record payment fee and tax components without altering payment principal.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_charge.h"
#include <string.h>
UmiStatus umi_payments_payment_charge_init(UmiPaymentsPaymentCharge *value,
    const char *id,
    const char *payment_id,
    int64_t fee_minor,
    int64_t tax_minor) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_payments_id_assign(&value->payment_id,payment_id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->fee_minor=fee_minor;
    value->tax_minor=tax_minor;
    return umi_payments_payment_charge_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_payments_payment_charge_valid(const UmiPaymentsPaymentCharge *value) {
    return value!=NULL && (value->fee_minor>=0 && value->tax_minor>=0);
}

int64_t umi_payments_payment_charge_total_minor(const UmiPaymentsPaymentCharge *value) {
    if(value==NULL) return (int64_t)0;
    return value->fee_minor+value->tax_minor;
}
