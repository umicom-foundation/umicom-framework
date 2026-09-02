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
/*
 * Initialise payments iso message from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_payments_iso_message_init(UmiPaymentsIsoMessage *value,
    const char *id,
    const char *payment_id,
    const char *message_family,
    const char *end_to_end_id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_payments_id_assign(&value->payment_id,payment_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_financial_core_copy(value->message_family,sizeof value->message_family,message_family);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    rc=umi_financial_core_copy(value->end_to_end_id,sizeof value->end_to_end_id,end_to_end_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    return umi_payments_iso_message_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments iso message satisfies its contract before another service relies on
 * it.
 */
bool umi_payments_iso_message_valid(const UmiPaymentsIsoMessage *value) {
    return value!=NULL && (value->message_family[0]!='\0' && value->end_to_end_id[0]!='\0');
}
