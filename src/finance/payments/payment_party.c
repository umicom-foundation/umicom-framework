/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_party.c
 *
 * PURPOSE:
 *   Implement represent canonical debtor/creditor identity and account routing data.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_party.h"
#include <string.h>
UmiStatus umi_payments_payment_party_init(UmiPaymentsPaymentParty *value,
    const char *id,
    const char *account_id,
    const char *display_name,
    const char *bank_code) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_payments_id_assign(&value->account_id,account_id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_financial_core_copy(value->display_name,sizeof value->display_name,display_name);
    if(rc!=UMI_STATUS_OK)return rc;
    rc=umi_financial_core_copy(value->bank_code,sizeof value->bank_code,bank_code);
    if(rc!=UMI_STATUS_OK)return rc;
    return umi_payments_payment_party_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_payments_payment_party_valid(const UmiPaymentsPaymentParty *value) {
    return value!=NULL && (value->display_name[0]!='\0' && value->bank_code[0]!='\0');
}
