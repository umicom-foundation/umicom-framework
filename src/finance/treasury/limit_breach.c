/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/limit_breach.c
 *
 * PURPOSE:
 *   Implement record risk-limit breaches and acknowledgement state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/limit_breach.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury limit breach from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_limit_breach_init(UmiTreasuryLimitBreach *value,
    const char *id,
    int64_t excess_minor,
    bool acknowledged) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->excess_minor=excess_minor;
    value->acknowledged=acknowledged;
    return umi_treasury_limit_breach_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury limit breach satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_limit_breach_valid(const UmiTreasuryLimitBreach *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->excess_minor > 0);
}

/*
 * Provide the treasury limit breach open operation used by this module and its client
 * applications.
 */
bool umi_treasury_limit_breach_open(const UmiTreasuryLimitBreach *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (bool)0;
    return !value->acknowledged;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryLimitBreachArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x27810787f99d6947);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryLimitBreach *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryLimitBreachArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryLimitBreach *)0)->id) - 1U +
        8U +
        8U;
}
static void UmiTreasuryLimitBreachArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryLimitBreach *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->excess_minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->acknowledged);
}
static void UmiTreasuryLimitBreachArchiveRead(UmiArchiveReader *reader, UmiTreasuryLimitBreach *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->excess_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->acknowledged = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiTreasuryLimitBreachArchiveValidate(const UmiTreasuryLimitBreach *value)
{
    return umi_treasury_limit_breach_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_limit_breach_archive_encode, umi_treasury_limit_breach_archive_decode,
    UmiTreasuryLimitBreach, UmiTreasuryLimitBreachArchiveSchema, UmiTreasuryLimitBreachArchiveBound, UmiTreasuryLimitBreachArchiveWrite, UmiTreasuryLimitBreachArchiveRead, UmiTreasuryLimitBreachArchiveValidate)
