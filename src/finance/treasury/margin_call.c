/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/margin_call.c
 *
 * PURPOSE:
 *   Implement represent a margin call amount, agreed amount and lifecycle state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/margin_call.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury margin call from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_margin_call_init(UmiTreasuryMarginCall *value,
    const char *id,
    int64_t called_minor,
    int64_t agreed_minor,
    UmiTreasuryMarginState state) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->called_minor=called_minor;
    value->agreed_minor=agreed_minor;
    value->state=state;
    return umi_treasury_margin_call_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury margin call satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_margin_call_valid(const UmiTreasuryMarginCall *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->called_minor >= 0 && value->agreed_minor >= 0 && value->agreed_minor <= value->called_minor && value->state >= UMI_TREASURY_MARGIN_OPEN && value->state <= UMI_TREASURY_MARGIN_SETTLED);
}

/*
 * Provide the treasury margin call unagreed minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_margin_call_unagreed_minor(const UmiTreasuryMarginCall *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->called_minor - value->agreed_minor;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryMarginCallArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7fd9b6b4fc33ce9a);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryMarginCall *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryMarginCallArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryMarginCall *)0)->id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiTreasuryMarginCallArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryMarginCall *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->called_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->agreed_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->state);
}
static void UmiTreasuryMarginCallArchiveRead(UmiArchiveReader *reader, UmiTreasuryMarginCall *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->called_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->agreed_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->state = (UmiTreasuryMarginState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiTreasuryMarginCallArchiveValidate(const UmiTreasuryMarginCall *value)
{
    return umi_treasury_margin_call_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_margin_call_archive_encode, umi_treasury_margin_call_archive_decode,
    UmiTreasuryMarginCall, UmiTreasuryMarginCallArchiveSchema, UmiTreasuryMarginCallArchiveBound, UmiTreasuryMarginCallArchiveWrite, UmiTreasuryMarginCallArchiveRead, UmiTreasuryMarginCallArchiveValidate)
