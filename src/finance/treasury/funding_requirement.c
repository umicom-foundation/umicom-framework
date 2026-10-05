/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/funding_requirement.c
 *
 * PURPOSE:
 *   Implement calculate a funding requirement from forecast outflows and available liquidity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/funding_requirement.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury funding requirement from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_funding_requirement_init(UmiTreasuryFundingRequirement *value,
    const char *id,
    int64_t required_liquidity_minor,
    int64_t available_liquidity_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->required_liquidity_minor=required_liquidity_minor;
    value->available_liquidity_minor=available_liquidity_minor;
    return umi_treasury_funding_requirement_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury funding requirement satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_funding_requirement_valid(const UmiTreasuryFundingRequirement *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->required_liquidity_minor >= 0 && value->available_liquidity_minor >= 0);
}

/*
 * Provide the treasury funding requirement shortfall minor operation used by this module
 * and its client applications.
 */
int64_t umi_treasury_funding_requirement_shortfall_minor(const UmiTreasuryFundingRequirement *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->required_liquidity_minor > value->available_liquidity_minor ? value->required_liquidity_minor - value->available_liquidity_minor : 0;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryFundingRequirementArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xdf939ceeef7a49f2);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryFundingRequirement *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryFundingRequirementArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryFundingRequirement *)0)->id) - 1U +
        8U +
        8U;
}
static void UmiTreasuryFundingRequirementArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryFundingRequirement *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->required_liquidity_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->available_liquidity_minor);
}
static void UmiTreasuryFundingRequirementArchiveRead(UmiArchiveReader *reader, UmiTreasuryFundingRequirement *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->required_liquidity_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->available_liquidity_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasuryFundingRequirementArchiveValidate(const UmiTreasuryFundingRequirement *value)
{
    return umi_treasury_funding_requirement_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_funding_requirement_archive_encode, umi_treasury_funding_requirement_archive_decode,
    UmiTreasuryFundingRequirement, UmiTreasuryFundingRequirementArchiveSchema, UmiTreasuryFundingRequirementArchiveBound, UmiTreasuryFundingRequirementArchiveWrite, UmiTreasuryFundingRequirementArchiveRead, UmiTreasuryFundingRequirementArchiveValidate)
