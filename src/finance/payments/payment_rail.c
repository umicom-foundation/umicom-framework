/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_rail.c
 *
 * PURPOSE:
 *   Implement describe payment-network capabilities and transaction limits.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_rail.h"
#include <string.h>
UmiStatus umi_payments_payment_rail_init(UmiPaymentsPaymentRail *value,
    const char *id,
    UmiPaymentsRailKind kind,
    const char *name,
    int64_t maximum_minor,
    bool supports_instant) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->kind=kind;
    rc=umi_financial_core_copy(value->name,sizeof value->name,name);
    if(rc!=UMI_STATUS_OK)return rc;
    value->maximum_minor=maximum_minor;
    value->supports_instant=supports_instant;
    return umi_payments_payment_rail_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_payments_payment_rail_valid(const UmiPaymentsPaymentRail *value) {
    return value!=NULL && (value->name[0]!='\0' && value->maximum_minor>0 && value->kind>=UMI_PAYMENTS_RAIL_INTERNAL && value->kind<=UMI_PAYMENTS_RAIL_CORRESPONDENT);
}

bool umi_payments_payment_rail_instant(const UmiPaymentsPaymentRail *value) {
    if(value==NULL) return (bool)0;
    return value->supports_instant;
}
