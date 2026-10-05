/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/settlement_obligation.c
 *
 * PURPOSE:
 *   Implement represent delivery-versus-payment settlement obligations and state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/settlement_obligation.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury settlement obligation from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_treasury_settlement_obligation_init(UmiTreasurySettlementObligation *value,
    const char *id,
    int64_t cash_minor,
    int64_t security_quantity,
    UmiTreasurySettlementState state) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->cash_minor=cash_minor;
    value->security_quantity=security_quantity;
    value->state=state;
    return umi_treasury_settlement_obligation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury settlement obligation satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_settlement_obligation_valid(const UmiTreasurySettlementObligation *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->cash_minor >= 0 && value->security_quantity >= 0 && value->state >= UMI_TREASURY_SETTLEMENT_NEW && value->state <= UMI_TREASURY_SETTLEMENT_CANCELLED);
}

/*
 * Provide the treasury settlement obligation complete operation used by this module and
 * its client applications.
 */
bool umi_treasury_settlement_obligation_complete(const UmiTreasurySettlementObligation *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (bool)0;
    return value->state == UMI_TREASURY_SETTLEMENT_SETTLED;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasurySettlementObligationArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc5bf0a846c18ba81);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasurySettlementObligation *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasurySettlementObligationArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasurySettlementObligation *)0)->id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiTreasurySettlementObligationArchiveWrite(UmiArchiveWriter *writer, const UmiTreasurySettlementObligation *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->cash_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->security_quantity);
    UmiArchiveWriteSigned(writer, (int64_t)value->state);
}
static void UmiTreasurySettlementObligationArchiveRead(UmiArchiveReader *reader, UmiTreasurySettlementObligation *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->cash_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->security_quantity = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->state = (UmiTreasurySettlementState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiTreasurySettlementObligationArchiveValidate(const UmiTreasurySettlementObligation *value)
{
    return umi_treasury_settlement_obligation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_settlement_obligation_archive_encode, umi_treasury_settlement_obligation_archive_decode,
    UmiTreasurySettlementObligation, UmiTreasurySettlementObligationArchiveSchema, UmiTreasurySettlementObligationArchiveBound, UmiTreasurySettlementObligationArchiveWrite, UmiTreasurySettlementObligationArchiveRead, UmiTreasurySettlementObligationArchiveValidate)
