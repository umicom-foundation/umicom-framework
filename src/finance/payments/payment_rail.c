/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_rail.c
 *
 * PURPOSE:
 *   Implement describe payment-network capabilities and transaction limits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_rail.h"
#include <string.h>
/*
 * Initialise payments payment rail from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_payments_payment_rail_init(UmiPaymentsPaymentRail *value,
    const char *id,
    UmiPaymentsRailKind kind,
    const char *name,
    int64_t maximum_minor,
    bool supports_instant) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->kind=kind;
    rc=umi_financial_core_copy(value->name,sizeof value->name,name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    value->maximum_minor=maximum_minor;
    value->supports_instant=supports_instant;
    return umi_payments_payment_rail_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments payment rail satisfies its contract before another service relies on
 * it.
 */
bool umi_payments_payment_rail_valid(const UmiPaymentsPaymentRail *value) {
    return value!=NULL && (value->name[0]!='\0' && value->maximum_minor>0 && value->kind>=UMI_PAYMENTS_RAIL_INTERNAL && value->kind<=UMI_PAYMENTS_RAIL_CORRESPONDENT);
}

/*
 * Provide the payments payment rail instant operation used by this module and its client
 * applications.
 */
bool umi_payments_payment_rail_instant(const UmiPaymentsPaymentRail *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->supports_instant;
}
