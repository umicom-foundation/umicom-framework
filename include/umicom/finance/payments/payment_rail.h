/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_rail.h
 *
 * PURPOSE:
 *   Describe payment-network capabilities and transaction limits.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_RAIL_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_RAIL_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPaymentsPaymentRail {
    UmiFinancialId id;
    UmiPaymentsRailKind kind;
    char name[UMI_PAYMENTS_NAME_CAPACITY];
    int64_t maximum_minor;
    bool supports_instant;
} UmiPaymentsPaymentRail;
UmiStatus umi_payments_payment_rail_init(UmiPaymentsPaymentRail *value,
    const char *id,
    UmiPaymentsRailKind kind,
    const char *name,
    int64_t maximum_minor,
    bool supports_instant);
bool umi_payments_payment_rail_valid(const UmiPaymentsPaymentRail *value);
bool umi_payments_payment_rail_instant(const UmiPaymentsPaymentRail *value);
#ifdef __cplusplus
}
#endif
#endif
