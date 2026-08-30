/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/iso_message.c
 *
 * PURPOSE:
 *   Implement represent ISO-20022-style business identifiers without binding to an XML parser.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/iso_message.h"
#include <string.h>
UmiStatus umi_payments_iso_message_init(UmiPaymentsIsoMessage *value,
    const char *id,
    const char *payment_id,
    const char *message_family,
    const char *end_to_end_id) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_payments_id_assign(&value->payment_id,payment_id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_financial_core_copy(value->message_family,sizeof value->message_family,message_family);
    if(rc!=UMI_STATUS_OK)return rc;
    rc=umi_financial_core_copy(value->end_to_end_id,sizeof value->end_to_end_id,end_to_end_id);
    if(rc!=UMI_STATUS_OK)return rc;
    return umi_payments_iso_message_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_payments_iso_message_valid(const UmiPaymentsIsoMessage *value) {
    return value!=NULL && (value->message_family[0]!='\0' && value->end_to_end_id[0]!='\0');
}
