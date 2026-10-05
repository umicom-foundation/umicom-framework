/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/eligibility_rule.c
 *
 * PURPOSE:
 *   Implement evaluate collateral eligibility using minimum value and maximum maturity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/eligibility_rule.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury eligibility rule from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_eligibility_rule_init(UmiTreasuryEligibilityRule *value,
    const char *id,
    int64_t minimum_value_minor,
    uint32_t maximum_maturity_days) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->minimum_value_minor=minimum_value_minor;
    value->maximum_maturity_days=maximum_maturity_days;
    return umi_treasury_eligibility_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury eligibility rule satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_eligibility_rule_valid(const UmiTreasuryEligibilityRule *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->minimum_value_minor >= 0 && value->maximum_maturity_days > 0U);
}

/*
 * Provide the treasury eligibility rule usable operation used by this module and its
 * client applications.
 */
bool umi_treasury_eligibility_rule_usable(const UmiTreasuryEligibilityRule *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (bool)0;
    return value->maximum_maturity_days > 0U;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryEligibilityRuleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xddc82978d305b03b);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryEligibilityRule *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryEligibilityRuleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryEligibilityRule *)0)->id) - 1U +
        8U +
        8U;
}
static void UmiTreasuryEligibilityRuleArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryEligibilityRule *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->minimum_value_minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->maximum_maturity_days);
}
static void UmiTreasuryEligibilityRuleArchiveRead(UmiArchiveReader *reader, UmiTreasuryEligibilityRule *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->minimum_value_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->maximum_maturity_days = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiTreasuryEligibilityRuleArchiveValidate(const UmiTreasuryEligibilityRule *value)
{
    return umi_treasury_eligibility_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_eligibility_rule_archive_encode, umi_treasury_eligibility_rule_archive_decode,
    UmiTreasuryEligibilityRule, UmiTreasuryEligibilityRuleArchiveSchema, UmiTreasuryEligibilityRuleArchiveBound, UmiTreasuryEligibilityRuleArchiveWrite, UmiTreasuryEligibilityRuleArchiveRead, UmiTreasuryEligibilityRuleArchiveValidate)
