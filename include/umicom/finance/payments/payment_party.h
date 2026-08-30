/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/payment_party.h
 *
 * PURPOSE:
 *   Represent canonical debtor/creditor identity and account routing data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_PAYMENT_PARTY_H
#define UMICOM_FINANCE_PAYMENTS_PAYMENT_PARTY_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPaymentsPaymentParty {
    UmiFinancialId id;
    UmiFinancialId account_id;
    char display_name[UMI_PAYMENTS_NAME_CAPACITY];
    char bank_code[UMI_FINANCIAL_CORE_CODE_CAPACITY];
} UmiPaymentsPaymentParty;
UmiStatus umi_payments_payment_party_init(UmiPaymentsPaymentParty *value,
    const char *id,
    const char *account_id,
    const char *display_name,
    const char *bank_code);
bool umi_payments_payment_party_valid(const UmiPaymentsPaymentParty *value);
#ifdef __cplusplus
}
#endif
#endif
