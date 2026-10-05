/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/payment_obligation.c
 *
 * PURPOSE:
 *   Implement represent a dated treasury payment obligation and outstanding amount.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/payment_obligation.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury payment obligation from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_payment_obligation_init(UmiTreasuryPaymentObligation *value,
    const char *id,
    int64_t due_epoch_millis,
    int64_t amount_minor,
    int64_t paid_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->due_epoch_millis=due_epoch_millis;
    value->amount_minor=amount_minor;
    value->paid_minor=paid_minor;
    return umi_treasury_payment_obligation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury payment obligation satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_payment_obligation_valid(const UmiTreasuryPaymentObligation *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->due_epoch_millis >= 0 && value->amount_minor >= 0 && value->paid_minor >= 0 && value->paid_minor <= value->amount_minor);
}

/*
 * Provide the treasury payment obligation outstanding minor operation used by this module
 * and its client applications.
 */
int64_t umi_treasury_payment_obligation_outstanding_minor(const UmiTreasuryPaymentObligation *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->amount_minor - value->paid_minor;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryPaymentObligationArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x08f0650eeb0af4d3);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryPaymentObligation *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryPaymentObligationArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryPaymentObligation *)0)->id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiTreasuryPaymentObligationArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryPaymentObligation *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->due_epoch_millis);
    UmiArchiveWriteSigned(writer, (int64_t)value->amount_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->paid_minor);
}
static void UmiTreasuryPaymentObligationArchiveRead(UmiArchiveReader *reader, UmiTreasuryPaymentObligation *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->due_epoch_millis = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->amount_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->paid_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasuryPaymentObligationArchiveValidate(const UmiTreasuryPaymentObligation *value)
{
    return umi_treasury_payment_obligation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_payment_obligation_archive_encode, umi_treasury_payment_obligation_archive_decode,
    UmiTreasuryPaymentObligation, UmiTreasuryPaymentObligationArchiveSchema, UmiTreasuryPaymentObligationArchiveBound, UmiTreasuryPaymentObligationArchiveWrite, UmiTreasuryPaymentObligationArchiveRead, UmiTreasuryPaymentObligationArchiveValidate)
