/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/haircut_rule.c
 *
 * PURPOSE:
 *   Implement define collateral valuation haircut in basis points.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/haircut_rule.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury haircut rule from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_haircut_rule_init(UmiTreasuryHaircutRule *value,
    const char *id,
    uint32_t haircut_bps) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->haircut_bps=haircut_bps;
    return umi_treasury_haircut_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury haircut rule satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_haircut_rule_valid(const UmiTreasuryHaircutRule *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->haircut_bps <= 10000U);
}

/*
 * Provide the treasury haircut rule remaining bps operation used by this module and its
 * client applications.
 */
uint32_t umi_treasury_haircut_rule_remaining_bps(const UmiTreasuryHaircutRule *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (uint32_t)0;
    return 10000U - value->haircut_bps;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryHaircutRuleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc25d67ac0fe5fb6a);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryHaircutRule *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryHaircutRuleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryHaircutRule *)0)->id) - 1U +
        8U;
}
static void UmiTreasuryHaircutRuleArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryHaircutRule *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->haircut_bps);
}
static void UmiTreasuryHaircutRuleArchiveRead(UmiArchiveReader *reader, UmiTreasuryHaircutRule *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->haircut_bps = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiTreasuryHaircutRuleArchiveValidate(const UmiTreasuryHaircutRule *value)
{
    return umi_treasury_haircut_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_haircut_rule_archive_encode, umi_treasury_haircut_rule_archive_decode,
    UmiTreasuryHaircutRule, UmiTreasuryHaircutRuleArchiveSchema, UmiTreasuryHaircutRuleArchiveBound, UmiTreasuryHaircutRuleArchiveWrite, UmiTreasuryHaircutRuleArchiveRead, UmiTreasuryHaircutRuleArchiveValidate)
