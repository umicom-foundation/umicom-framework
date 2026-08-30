/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/iso_message.h
 *
 * PURPOSE:
 *   Represent ISO-20022-style business identifiers without binding to an XML parser.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_ISO_MESSAGE_H
#define UMICOM_FINANCE_PAYMENTS_ISO_MESSAGE_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPaymentsIsoMessage {
    UmiFinancialId id;
    UmiFinancialId payment_id;
    char message_family[UMI_FINANCIAL_CORE_CODE_CAPACITY];
    char end_to_end_id[UMI_PAYMENTS_ID_CAPACITY];
} UmiPaymentsIsoMessage;
UmiStatus umi_payments_iso_message_init(UmiPaymentsIsoMessage *value,
    const char *id,
    const char *payment_id,
    const char *message_family,
    const char *end_to_end_id);
bool umi_payments_iso_message_valid(const UmiPaymentsIsoMessage *value);
#ifdef __cplusplus
}
#endif
#endif
