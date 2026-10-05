/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_return.c
 *
 * PURPOSE:
 *   Implement represent full or partial payment returns with bounded reason codes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_return.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise payments payment return from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_return_init(UmiPaymentsPaymentReturn *value,
    const char *id,
    const char *original_payment_id,
    const char *reason_code,
    int64_t amount_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_payments_id_assign(&value->original_payment_id,original_payment_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_financial_core_copy(value->reason_code,sizeof value->reason_code,reason_code);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    value->amount_minor=amount_minor;
    return umi_payments_payment_return_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments payment return satisfies its contract before another service relies
 * on it.
 */
bool umi_payments_payment_return_valid(const UmiPaymentsPaymentReturn *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->original_payment_id.value, '\0', sizeof(value->original_payment_id.value)) == NULL) return 0;
    if (memchr(value->reason_code, '\0', sizeof(value->reason_code)) == NULL) return 0;

    return value!=NULL && (value->reason_code[0]!='\0' && value->amount_minor>0);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiPaymentsPaymentReturnArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x2e9f4f7673d39fe4);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsPaymentReturn *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsPaymentReturn *)0)->original_payment_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsPaymentReturn *)0)->reason_code)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPaymentsPaymentReturnArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiPaymentsPaymentReturn *)0)->id.value) - 1U +
        8U + sizeof(((UmiPaymentsPaymentReturn *)0)->original_payment_id.value) - 1U +
        8U + sizeof(((UmiPaymentsPaymentReturn *)0)->reason_code) - 1U +
        8U;
}
static void UmiPaymentsPaymentReturnArchiveWrite(UmiArchiveWriter *writer, const UmiPaymentsPaymentReturn *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->original_payment_id.value, sizeof(value->original_payment_id.value));
    UmiArchiveWriteText(writer, value->reason_code, sizeof(value->reason_code));
    UmiArchiveWriteSigned(writer, (int64_t)value->amount_minor);
}
static void UmiPaymentsPaymentReturnArchiveRead(UmiArchiveReader *reader, UmiPaymentsPaymentReturn *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->original_payment_id.value, sizeof(value->original_payment_id.value));
    UmiArchiveReadText(reader, value->reason_code, sizeof(value->reason_code));
    value->amount_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiPaymentsPaymentReturnArchiveValidate(const UmiPaymentsPaymentReturn *value)
{
    return umi_payments_payment_return_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_payments_payment_return_archive_encode, umi_payments_payment_return_archive_decode,
    UmiPaymentsPaymentReturn, UmiPaymentsPaymentReturnArchiveSchema, UmiPaymentsPaymentReturnArchiveBound, UmiPaymentsPaymentReturnArchiveWrite, UmiPaymentsPaymentReturnArchiveRead, UmiPaymentsPaymentReturnArchiveValidate)
