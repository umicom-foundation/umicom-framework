/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/settlement_window.c
 *
 * PURPOSE:
 *   Implement define operational settlement opening and cut-off timestamps.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/settlement_window.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury settlement window from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_settlement_window_init(UmiTreasurySettlementWindow *value,
    const char *id,
    int64_t opens_epoch_millis,
    int64_t closes_epoch_millis) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->opens_epoch_millis=opens_epoch_millis;
    value->closes_epoch_millis=closes_epoch_millis;
    return umi_treasury_settlement_window_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury settlement window satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_settlement_window_valid(const UmiTreasurySettlementWindow *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->opens_epoch_millis >= 0 && value->closes_epoch_millis >= value->opens_epoch_millis);
}

/*
 * Provide the treasury settlement window duration millis operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_settlement_window_duration_millis(const UmiTreasurySettlementWindow *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->closes_epoch_millis - value->opens_epoch_millis;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasurySettlementWindowArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x0a7bd7ad5b916e40);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasurySettlementWindow *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasurySettlementWindowArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasurySettlementWindow *)0)->id) - 1U +
        8U +
        8U;
}
static void UmiTreasurySettlementWindowArchiveWrite(UmiArchiveWriter *writer, const UmiTreasurySettlementWindow *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->opens_epoch_millis);
    UmiArchiveWriteSigned(writer, (int64_t)value->closes_epoch_millis);
}
static void UmiTreasurySettlementWindowArchiveRead(UmiArchiveReader *reader, UmiTreasurySettlementWindow *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->opens_epoch_millis = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->closes_epoch_millis = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasurySettlementWindowArchiveValidate(const UmiTreasurySettlementWindow *value)
{
    return umi_treasury_settlement_window_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_settlement_window_archive_encode, umi_treasury_settlement_window_archive_decode,
    UmiTreasurySettlementWindow, UmiTreasurySettlementWindowArchiveSchema, UmiTreasurySettlementWindowArchiveBound, UmiTreasurySettlementWindowArchiveWrite, UmiTreasurySettlementWindowArchiveRead, UmiTreasurySettlementWindowArchiveValidate)
