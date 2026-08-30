/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_instruction.c
 *
 * PURPOSE:
 *   Implement represent canonical payment economic meaning, parties, amount and lifecycle state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_instruction.h"
#include <string.h>
UmiStatus umi_payments_payment_instruction_init(UmiPaymentsPaymentInstruction *value,
    const char *id,
    const char *debtor_party_id,
    const char *creditor_party_id,
    const char *currency_code,
    int64_t amount_minor,
    UmiFinancialDate requested_date,
    UmiPaymentsStatus status,
    const char *idempotency_key) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_payments_id_assign(&value->debtor_party_id,debtor_party_id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_payments_id_assign(&value->creditor_party_id,creditor_party_id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_payments_currency_from_code(currency_code,&value->currency);
    if(rc!=UMI_STATUS_OK)return rc;
    value->amount_minor=amount_minor;
    value->requested_date=requested_date;
    value->status=status;
    rc=umi_financial_core_copy(value->idempotency_key,sizeof value->idempotency_key,idempotency_key);
    if(rc!=UMI_STATUS_OK)return rc;
    return umi_payments_payment_instruction_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_payments_payment_instruction_valid(const UmiPaymentsPaymentInstruction *value) {
    return value!=NULL && (value->amount_minor>0 && umi_financial_date_is_valid(value->requested_date) && value->status>=UMI_PAYMENTS_CREATED && value->status<=UMI_PAYMENTS_REJECTED && value->idempotency_key[0]!='\0');
}

bool umi_payments_payment_instruction_final(const UmiPaymentsPaymentInstruction *value) {
    if(value==NULL) return (bool)0;
    return value->status==UMI_PAYMENTS_SETTLED || value->status==UMI_PAYMENTS_RETURNED || value->status==UMI_PAYMENTS_REJECTED;
}
