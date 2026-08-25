/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_message.h
 *
 * PURPOSE:
 *   Represent durable canonical payment message metadata and direction.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_MESSAGE_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_MESSAGE_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPaymentsPaymentMessage {
    UmiFinancialId id;
    UmiFinancialId payment_id;
    char message_type[UMI_FINANCIAL_CORE_CODE_CAPACITY];
    UmiPaymentsMessageDirection direction;
    uint64_t sequence;
} UmiPaymentsPaymentMessage;
UmiStatus umi_payments_payment_message_init(UmiPaymentsPaymentMessage *value,
    const char *id,
    const char *payment_id,
    const char *message_type,
    UmiPaymentsMessageDirection direction,
    uint64_t sequence);
bool umi_payments_payment_message_valid(const UmiPaymentsPaymentMessage *value);
bool umi_payments_payment_message_outbound(const UmiPaymentsPaymentMessage *value);
#ifdef __cplusplus
}
#endif
#endif
