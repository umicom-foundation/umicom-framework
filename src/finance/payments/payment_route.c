/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_route.c
 *
 * PURPOSE:
 *   Implement represent selected payment rail, routing priority and route availability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_route.h"
#include <string.h>
UmiStatus umi_payments_payment_route_init(UmiPaymentsPaymentRoute *value,
    const char *id,
    UmiPaymentsRailKind rail_kind,
    uint32_t priority,
    bool available) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->rail_kind=rail_kind;
    value->priority=priority;
    value->available=available;
    return umi_payments_payment_route_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_payments_payment_route_valid(const UmiPaymentsPaymentRoute *value) {
    return value!=NULL && (value->rail_kind>=UMI_PAYMENTS_RAIL_INTERNAL && value->rail_kind<=UMI_PAYMENTS_RAIL_CORRESPONDENT);
}

bool umi_payments_payment_route_usable(const UmiPaymentsPaymentRoute *value) {
    if(value==NULL) return (bool)0;
    return value->available;
}
