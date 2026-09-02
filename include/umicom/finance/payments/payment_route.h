/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_route.h
 *
 * PURPOSE:
 *   Represent selected payment rail, routing priority and route availability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_ROUTE_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_ROUTE_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the payments payment route data shared with callers of this public contract.
 */
typedef struct UmiPaymentsPaymentRoute {
    UmiFinancialId id;
    UmiPaymentsRailKind rail_kind;
    uint32_t priority;
    bool available;
} UmiPaymentsPaymentRoute;
/**
 * Initialise payments payment route from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_route_init(UmiPaymentsPaymentRoute *value,
    const char *id,
    UmiPaymentsRailKind rail_kind,
    uint32_t priority,
    bool available);
/**
 * Check that payments payment route satisfies its contract before another service relies
 * on it.
 */
bool umi_payments_payment_route_valid(const UmiPaymentsPaymentRoute *value);
/**
 * Provide the payments payment route usable operation used by this module and its client
 * applications.
 */
bool umi_payments_payment_route_usable(const UmiPaymentsPaymentRoute *value);
#ifdef __cplusplus
}
#endif
#endif
