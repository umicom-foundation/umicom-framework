/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_route.c
 *
 * PURPOSE:
 *   Implement represent selected payment rail, routing priority and route availability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_route.h"
#include <string.h>
/*
 * Initialise payments payment route from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_route_init(UmiPaymentsPaymentRoute *value,
    const char *id,
    UmiPaymentsRailKind rail_kind,
    uint32_t priority,
    bool available) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->rail_kind=rail_kind;
    value->priority=priority;
    value->available=available;
    return umi_payments_payment_route_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments payment route satisfies its contract before another service relies
 * on it.
 */
bool umi_payments_payment_route_valid(const UmiPaymentsPaymentRoute *value) {
    return value!=NULL && (value->rail_kind>=UMI_PAYMENTS_RAIL_INTERNAL && value->rail_kind<=UMI_PAYMENTS_RAIL_CORRESPONDENT);
}

/*
 * Provide the payments payment route usable operation used by this module and its client
 * applications.
 */
bool umi_payments_payment_route_usable(const UmiPaymentsPaymentRoute *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->available;
}
