/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/treasury_event.c
 *
 * PURPOSE:
 *   Implement record sequence-ordered treasury domain events with event timestamp.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/treasury_event.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury treasury event from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_treasury_event_init(UmiTreasuryTreasuryEvent *value,
    const char *id,
    uint64_t sequence,
    int64_t event_epoch_millis) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->sequence=sequence;
    value->event_epoch_millis=event_epoch_millis;
    return umi_treasury_treasury_event_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury treasury event satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_treasury_event_valid(const UmiTreasuryTreasuryEvent *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->sequence > 0U && value->event_epoch_millis >= 0);
}

/*
 * Provide the treasury treasury event event sequence operation used by this module and its
 * client applications.
 */
uint64_t umi_treasury_treasury_event_event_sequence(const UmiTreasuryTreasuryEvent *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (uint64_t)0;
    return value->sequence;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryTreasuryEventArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xfeccc33a5aaebec2);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryTreasuryEvent *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryTreasuryEventArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryTreasuryEvent *)0)->id) - 1U +
        8U +
        8U;
}
static void UmiTreasuryTreasuryEventArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryTreasuryEvent *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
    UmiArchiveWriteSigned(writer, (int64_t)value->event_epoch_millis);
}
static void UmiTreasuryTreasuryEventArchiveRead(UmiArchiveReader *reader, UmiTreasuryTreasuryEvent *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->event_epoch_millis = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasuryTreasuryEventArchiveValidate(const UmiTreasuryTreasuryEvent *value)
{
    return umi_treasury_treasury_event_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_treasury_event_archive_encode, umi_treasury_treasury_event_archive_decode,
    UmiTreasuryTreasuryEvent, UmiTreasuryTreasuryEventArchiveSchema, UmiTreasuryTreasuryEventArchiveBound, UmiTreasuryTreasuryEventArchiveWrite, UmiTreasuryTreasuryEventArchiveRead, UmiTreasuryTreasuryEventArchiveValidate)
