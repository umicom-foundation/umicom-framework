/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/payments/payment_clearing.c
 *
 * PURPOSE:
 *   Implement represent gross/net clearing values and participant count for payment rails.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_clearing.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise payments payment clearing from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_payment_clearing_init(UmiPaymentsPaymentClearing *value,
    const char *id,
    int64_t gross_minor,
    int64_t net_minor,
    size_t participant_count,
    bool complete) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_payments_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->gross_minor=gross_minor;
    value->net_minor=net_minor;
    value->participant_count=participant_count;
    value->complete=complete;
    return umi_payments_payment_clearing_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that payments payment clearing satisfies its contract before another service
 * relies on it.
 */
bool umi_payments_payment_clearing_valid(const UmiPaymentsPaymentClearing *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;

    return value!=NULL && (value->gross_minor>=0 && umi_payments_abs_i64(value->net_minor)<=value->gross_minor && value->participant_count>0U);
}

/*
 * Provide the payments payment clearing cleared operation used by this module and its
 * client applications.
 */
bool umi_payments_payment_clearing_cleared(const UmiPaymentsPaymentClearing *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->complete;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiPaymentsPaymentClearingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x542c19a3833878a3);
    schema = (schema ^ (uint64_t)sizeof(((UmiPaymentsPaymentClearing *)0)->id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPaymentsPaymentClearingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiPaymentsPaymentClearing *)0)->id.value) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiPaymentsPaymentClearingArchiveWrite(UmiArchiveWriter *writer, const UmiPaymentsPaymentClearing *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteSigned(writer, (int64_t)value->gross_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->net_minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->participant_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->complete);
}
static void UmiPaymentsPaymentClearingArchiveRead(UmiArchiveReader *reader, UmiPaymentsPaymentClearing *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    value->gross_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->net_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->participant_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->complete = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiPaymentsPaymentClearingArchiveValidate(const UmiPaymentsPaymentClearing *value)
{
    return umi_payments_payment_clearing_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_payments_payment_clearing_archive_encode, umi_payments_payment_clearing_archive_decode,
    UmiPaymentsPaymentClearing, UmiPaymentsPaymentClearingArchiveSchema, UmiPaymentsPaymentClearingArchiveBound, UmiPaymentsPaymentClearingArchiveWrite, UmiPaymentsPaymentClearingArchiveRead, UmiPaymentsPaymentClearingArchiveValidate)
