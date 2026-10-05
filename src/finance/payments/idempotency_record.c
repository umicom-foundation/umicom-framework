/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/idempotency_record.c
 *
 * PURPOSE:
 *   Implement bind an idempotency key and request fingerprint to one canonical payment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/idempotency_record.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise payments idempotency record from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_idempotency_record_init(UmiPaymentsIdempotencyRecord *value,
    const char *id,
    const char *payment_id,
    const char *idempotency_key,
    uint64_t fingerprint) {
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
    rc=umi_financial_core_copy(value->idempotency_key,sizeof value->idempotency_key,idempotency_key);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    value->fingerprint=fingerprint;
    return umi_payments_idempotency_record_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments idempotency record satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_idempotency_record_valid(const UmiPaymentsIdempotencyRecord *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->payment_id.value, '\0', sizeof(value->payment_id.value)) == NULL) return 0;
    if (memchr(value->idempotency_key, '\0', sizeof(value->idempotency_key)) == NULL) return 0;

    return value!=NULL && (value->idempotency_key[0]!='\0' && value->fingerprint!=0U);
}

/*
 * Provide the payments idempotency record replay safe operation used by this module and
 * its client applications.
 */
bool umi_payments_idempotency_record_replay_safe(const UmiPaymentsIdempotencyRecord *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->fingerprint!=0U;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiPaymentsIdempotencyRecordArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x42a4ed4008150ac6);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsIdempotencyRecord *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsIdempotencyRecord *)0)->payment_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsIdempotencyRecord *)0)->idempotency_key)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPaymentsIdempotencyRecordArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiPaymentsIdempotencyRecord *)0)->id.value) - 1U +
        8U + sizeof(((UmiPaymentsIdempotencyRecord *)0)->payment_id.value) - 1U +
        8U + sizeof(((UmiPaymentsIdempotencyRecord *)0)->idempotency_key) - 1U +
        8U;
}
static void UmiPaymentsIdempotencyRecordArchiveWrite(UmiArchiveWriter *writer, const UmiPaymentsIdempotencyRecord *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->payment_id.value, sizeof(value->payment_id.value));
    UmiArchiveWriteText(writer, value->idempotency_key, sizeof(value->idempotency_key));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->fingerprint);
}
static void UmiPaymentsIdempotencyRecordArchiveRead(UmiArchiveReader *reader, UmiPaymentsIdempotencyRecord *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->payment_id.value, sizeof(value->payment_id.value));
    UmiArchiveReadText(reader, value->idempotency_key, sizeof(value->idempotency_key));
    value->fingerprint = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiPaymentsIdempotencyRecordArchiveValidate(const UmiPaymentsIdempotencyRecord *value)
{
    return umi_payments_idempotency_record_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_payments_idempotency_record_archive_encode, umi_payments_idempotency_record_archive_decode,
    UmiPaymentsIdempotencyRecord, UmiPaymentsIdempotencyRecordArchiveSchema, UmiPaymentsIdempotencyRecordArchiveBound, UmiPaymentsIdempotencyRecordArchiveWrite, UmiPaymentsIdempotencyRecordArchiveRead, UmiPaymentsIdempotencyRecordArchiveValidate)
