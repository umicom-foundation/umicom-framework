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
#include "../../base/value_archive_internal.h"
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
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->payment_id.value, '\0', sizeof(value->payment_id.value)) == NULL) return 0;
    if (memchr(value->message_family, '\0', sizeof(value->message_family)) == NULL) return 0;
    if (memchr(value->end_to_end_id, '\0', sizeof(value->end_to_end_id)) == NULL) return 0;

    return value!=NULL && (value->message_family[0]!='\0' && value->end_to_end_id[0]!='\0');
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiPaymentsIsoMessageArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xe5c2f191e0e79839);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsIsoMessage *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsIsoMessage *)0)->payment_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsIsoMessage *)0)->message_family)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsIsoMessage *)0)->end_to_end_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPaymentsIsoMessageArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiPaymentsIsoMessage *)0)->id.value) - 1U +
        8U + sizeof(((UmiPaymentsIsoMessage *)0)->payment_id.value) - 1U +
        8U + sizeof(((UmiPaymentsIsoMessage *)0)->message_family) - 1U +
        8U + sizeof(((UmiPaymentsIsoMessage *)0)->end_to_end_id) - 1U;
}
static void UmiPaymentsIsoMessageArchiveWrite(UmiArchiveWriter *writer, const UmiPaymentsIsoMessage *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->payment_id.value, sizeof(value->payment_id.value));
    UmiArchiveWriteText(writer, value->message_family, sizeof(value->message_family));
    UmiArchiveWriteText(writer, value->end_to_end_id, sizeof(value->end_to_end_id));
}
static void UmiPaymentsIsoMessageArchiveRead(UmiArchiveReader *reader, UmiPaymentsIsoMessage *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->payment_id.value, sizeof(value->payment_id.value));
    UmiArchiveReadText(reader, value->message_family, sizeof(value->message_family));
    UmiArchiveReadText(reader, value->end_to_end_id, sizeof(value->end_to_end_id));
}
static UmiStatus UmiPaymentsIsoMessageArchiveValidate(const UmiPaymentsIsoMessage *value)
{
    return umi_payments_iso_message_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_payments_iso_message_archive_encode, umi_payments_iso_message_archive_decode,
    UmiPaymentsIsoMessage, UmiPaymentsIsoMessageArchiveSchema, UmiPaymentsIsoMessageArchiveBound, UmiPaymentsIsoMessageArchiveWrite, UmiPaymentsIsoMessageArchiveRead, UmiPaymentsIsoMessageArchiveValidate)
