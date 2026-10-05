/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/collateral_allocation.c
 *
 * PURPOSE:
 *   Implement represent collateral allocation against a secured requirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/collateral_allocation.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury collateral allocation from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_treasury_collateral_allocation_init(UmiTreasuryCollateralAllocation *value,
    const char *id,
    int64_t required_minor,
    int64_t allocated_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->required_minor=required_minor;
    value->allocated_minor=allocated_minor;
    return umi_treasury_collateral_allocation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury collateral allocation satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_collateral_allocation_valid(const UmiTreasuryCollateralAllocation *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->required_minor >= 0 && value->allocated_minor >= 0);
}

/*
 * Provide the treasury collateral allocation shortfall minor operation used by this module
 * and its client applications.
 */
int64_t umi_treasury_collateral_allocation_shortfall_minor(const UmiTreasuryCollateralAllocation *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->required_minor > value->allocated_minor ? value->required_minor - value->allocated_minor : 0;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryCollateralAllocationArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x075c95fc98066d3a);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryCollateralAllocation *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryCollateralAllocationArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryCollateralAllocation *)0)->id) - 1U +
        8U +
        8U;
}
static void UmiTreasuryCollateralAllocationArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryCollateralAllocation *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->required_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->allocated_minor);
}
static void UmiTreasuryCollateralAllocationArchiveRead(UmiArchiveReader *reader, UmiTreasuryCollateralAllocation *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->required_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->allocated_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasuryCollateralAllocationArchiveValidate(const UmiTreasuryCollateralAllocation *value)
{
    return umi_treasury_collateral_allocation_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_collateral_allocation_archive_encode, umi_treasury_collateral_allocation_archive_decode,
    UmiTreasuryCollateralAllocation, UmiTreasuryCollateralAllocationArchiveSchema, UmiTreasuryCollateralAllocationArchiveBound, UmiTreasuryCollateralAllocationArchiveWrite, UmiTreasuryCollateralAllocationArchiveRead, UmiTreasuryCollateralAllocationArchiveValidate)
