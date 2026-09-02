/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_rail.h
 *
 * PURPOSE:
 *   Describe payment-network capabilities and transaction limits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_RAIL_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_RAIL_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the payments payment rail data shared with callers of this public contract.
 */
typedef struct UmiPaymentsPaymentRail {
    UmiFinancialId id;
    UmiPaymentsRailKind kind;
    char name[UMI_PAYMENTS_NAME_CAPACITY];
    int64_t maximum_minor;
    bool supports_instant;
} UmiPaymentsPaymentRail;
/**
 * Initialise payments payment rail from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_payments_payment_rail_init(UmiPaymentsPaymentRail *value,
    const char *id,
    UmiPaymentsRailKind kind,
    const char *name,
    int64_t maximum_minor,
    bool supports_instant);
/**
 * Check that payments payment rail satisfies its contract before another service relies on
 * it.
 */
bool umi_payments_payment_rail_valid(const UmiPaymentsPaymentRail *value);
/**
 * Provide the payments payment rail instant operation used by this module and its client
 * applications.
 */
bool umi_payments_payment_rail_instant(const UmiPaymentsPaymentRail *value);
#ifdef __cplusplus
}
#endif
#endif
