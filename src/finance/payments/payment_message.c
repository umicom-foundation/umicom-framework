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
#include "../../base/value_archive_internal.h"
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
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->payment_id.value, '\0', sizeof(value->payment_id.value)) == NULL) return 0;
    if (memchr(value->message_type, '\0', sizeof(value->message_type)) == NULL) return 0;

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

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiPaymentsPaymentMessageArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x46959200401ccaff);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsPaymentMessage *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsPaymentMessage *)0)->payment_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsPaymentMessage *)0)->message_type)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPaymentsPaymentMessageArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiPaymentsPaymentMessage *)0)->id.value) - 1U +
        8U + sizeof(((UmiPaymentsPaymentMessage *)0)->payment_id.value) - 1U +
        8U + sizeof(((UmiPaymentsPaymentMessage *)0)->message_type) - 1U +
        8U +
        8U;
}
static void UmiPaymentsPaymentMessageArchiveWrite(UmiArchiveWriter *writer, const UmiPaymentsPaymentMessage *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->payment_id.value, sizeof(value->payment_id.value));
    UmiArchiveWriteText(writer, value->message_type, sizeof(value->message_type));
    UmiArchiveWriteSigned(writer, (int64_t)value->direction);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
}
static void UmiPaymentsPaymentMessageArchiveRead(UmiArchiveReader *reader, UmiPaymentsPaymentMessage *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->payment_id.value, sizeof(value->payment_id.value));
    UmiArchiveReadText(reader, value->message_type, sizeof(value->message_type));
    value->direction = (UmiPaymentsMessageDirection)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiPaymentsPaymentMessageArchiveValidate(const UmiPaymentsPaymentMessage *value)
{
    return umi_payments_payment_message_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_payments_payment_message_archive_encode, umi_payments_payment_message_archive_decode,
    UmiPaymentsPaymentMessage, UmiPaymentsPaymentMessageArchiveSchema, UmiPaymentsPaymentMessageArchiveBound, UmiPaymentsPaymentMessageArchiveWrite, UmiPaymentsPaymentMessageArchiveRead, UmiPaymentsPaymentMessageArchiveValidate)
