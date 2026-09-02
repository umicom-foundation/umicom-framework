/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_message.h
 *
 * PURPOSE:
 *   Represent durable canonical payment message metadata and direction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_MESSAGE_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_MESSAGE_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the payments payment message data shared with callers of this public contract.
 */
typedef struct UmiPaymentsPaymentMessage {
    UmiFinancialId id;
    UmiFinancialId payment_id;
    char message_type[UMI_FINANCIAL_CORE_CODE_CAPACITY];
    UmiPaymentsMessageDirection direction;
    uint64_t sequence;
} UmiPaymentsPaymentMessage;
/**
 * Initialise payments payment message from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_message_init(UmiPaymentsPaymentMessage *value,
    const char *id,
    const char *payment_id,
    const char *message_type,
    UmiPaymentsMessageDirection direction,
    uint64_t sequence);
/**
 * Check that payments payment message satisfies its contract before another service relies
 * on it.
 */
bool umi_payments_payment_message_valid(const UmiPaymentsPaymentMessage *value);
/**
 * Provide the payments payment message outbound operation used by this module and its
 * client applications.
 */
bool umi_payments_payment_message_outbound(const UmiPaymentsPaymentMessage *value);
#ifdef __cplusplus
}
#endif
#endif
