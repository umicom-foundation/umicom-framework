/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_route.h
 *
 * PURPOSE:
 *   Represent selected payment rail, routing priority and route availability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_ROUTE_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_ROUTE_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPaymentsPaymentRoute {
    UmiFinancialId id;
    UmiPaymentsRailKind rail_kind;
    uint32_t priority;
    bool available;
} UmiPaymentsPaymentRoute;
UmiStatus umi_payments_payment_route_init(UmiPaymentsPaymentRoute *value,
    const char *id,
    UmiPaymentsRailKind rail_kind,
    uint32_t priority,
    bool available);
bool umi_payments_payment_route_valid(const UmiPaymentsPaymentRoute *value);
bool umi_payments_payment_route_usable(const UmiPaymentsPaymentRoute *value);
#ifdef __cplusplus
}
#endif
#endif
