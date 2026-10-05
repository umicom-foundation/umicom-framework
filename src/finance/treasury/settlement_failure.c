/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/settlement_failure.c
 *
 * PURPOSE:
 *   Implement record failed settlement exposure, age and retry eligibility.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/settlement_failure.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury settlement failure from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_settlement_failure_init(UmiTreasurySettlementFailure *value,
    const char *id,
    int64_t exposure_minor,
    uint32_t age_days,
    uint32_t retry_count) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->exposure_minor=exposure_minor;
    value->age_days=age_days;
    value->retry_count=retry_count;
    return umi_treasury_settlement_failure_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury settlement failure satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_settlement_failure_valid(const UmiTreasurySettlementFailure *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->exposure_minor >= 0);
}

/*
 * Provide the treasury settlement failure aged operation used by this module and its
 * client applications.
 */
bool umi_treasury_settlement_failure_aged(const UmiTreasurySettlementFailure *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (bool)0;
    return value->age_days > 2U;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasurySettlementFailureArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7200ef6cb5deb33a);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasurySettlementFailure *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasurySettlementFailureArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasurySettlementFailure *)0)->id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiTreasurySettlementFailureArchiveWrite(UmiArchiveWriter *writer, const UmiTreasurySettlementFailure *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->exposure_minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->age_days);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->retry_count);
}
static void UmiTreasurySettlementFailureArchiveRead(UmiArchiveReader *reader, UmiTreasurySettlementFailure *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->exposure_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->age_days = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->retry_count = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiTreasurySettlementFailureArchiveValidate(const UmiTreasurySettlementFailure *value)
{
    return umi_treasury_settlement_failure_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_settlement_failure_archive_encode, umi_treasury_settlement_failure_archive_decode,
    UmiTreasurySettlementFailure, UmiTreasurySettlementFailureArchiveSchema, UmiTreasurySettlementFailureArchiveBound, UmiTreasurySettlementFailureArchiveWrite, UmiTreasurySettlementFailureArchiveRead, UmiTreasurySettlementFailureArchiveValidate)
