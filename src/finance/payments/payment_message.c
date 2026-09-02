/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_message.c
 *
 * PURPOSE:
 *   Implement represent durable canonical payment message metadata and direction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_message.h"
#include <string.h>
/*
 * Initialise payments payment message from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_message_init(UmiPaymentsPaymentMessage *value,
    const char *id,
    const char *payment_id,
    const char *message_type,
    UmiPaymentsMessageDirection direction,
    uint64_t sequence) {
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
    rc=umi_financial_core_copy(value->message_type,sizeof value->message_type,message_type);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    value->direction=direction;
    value->sequence=sequence;
    return umi_payments_payment_message_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments payment message satisfies its contract before another service relies
 * on it.
 */
bool umi_payments_payment_message_valid(const UmiPaymentsPaymentMessage *value) {
    return value!=NULL && (value->message_type[0]!='\0' && value->sequence>0U && (value->direction==UMI_PAYMENTS_MESSAGE_OUTBOUND||value->direction==UMI_PAYMENTS_MESSAGE_INBOUND));
}

/*
 * Provide the payments payment message outbound operation used by this module and its
 * client applications.
 */
bool umi_payments_payment_message_outbound(const UmiPaymentsPaymentMessage *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->direction==UMI_PAYMENTS_MESSAGE_OUTBOUND;
}
