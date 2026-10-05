/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/collateral_valuation.c
 *
 * PURPOSE:
 *   Implement calculate post-haircut collateral value.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/collateral_valuation.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury collateral valuation from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_collateral_valuation_init(UmiTreasuryCollateralValuation *value,
    const char *id,
    int64_t gross_value_minor,
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
    value->gross_value_minor=gross_value_minor;
    value->haircut_bps=haircut_bps;
    return umi_treasury_collateral_valuation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury collateral valuation satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_collateral_valuation_valid(const UmiTreasuryCollateralValuation *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->gross_value_minor >= 0 && value->haircut_bps <= 10000U);
}

/*
 * Provide the treasury collateral valuation eligible value minor operation used by this
 * module and its client applications.
 */
int64_t umi_treasury_collateral_valuation_eligible_value_minor(const UmiTreasuryCollateralValuation *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return (value->gross_value_minor * (int64_t)(10000U - value->haircut_bps)) / 10000;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryCollateralValuationArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x6a37c55b6b05f54a);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryCollateralValuation *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryCollateralValuationArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryCollateralValuation *)0)->id) - 1U +
        8U +
        8U;
}
static void UmiTreasuryCollateralValuationArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryCollateralValuation *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->gross_value_minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->haircut_bps);
}
static void UmiTreasuryCollateralValuationArchiveRead(UmiArchiveReader *reader, UmiTreasuryCollateralValuation *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->gross_value_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->haircut_bps = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiTreasuryCollateralValuationArchiveValidate(const UmiTreasuryCollateralValuation *value)
{
    return umi_treasury_collateral_valuation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_collateral_valuation_archive_encode, umi_treasury_collateral_valuation_archive_decode,
    UmiTreasuryCollateralValuation, UmiTreasuryCollateralValuationArchiveSchema, UmiTreasuryCollateralValuationArchiveBound, UmiTreasuryCollateralValuationArchiveWrite, UmiTreasuryCollateralValuationArchiveRead, UmiTreasuryCollateralValuationArchiveValidate)
