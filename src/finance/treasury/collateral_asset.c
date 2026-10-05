/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/collateral_asset.c
 *
 * PURPOSE:
 *   Implement describe collateral inventory quantity, price and collateral kind.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/collateral_asset.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury collateral asset from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_collateral_asset_init(UmiTreasuryCollateralAsset *value,
    const char *id,
    UmiTreasuryCollateralKind kind,
    int64_t quantity,
    int64_t unit_value_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->kind=kind;
    value->quantity=quantity;
    value->unit_value_minor=unit_value_minor;
    return umi_treasury_collateral_asset_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury collateral asset satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_collateral_asset_valid(const UmiTreasuryCollateralAsset *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->kind >= UMI_TREASURY_COLLATERAL_CASH && value->kind <= UMI_TREASURY_COLLATERAL_SECURITY && value->quantity >= 0 && value->unit_value_minor >= 0);
}

/*
 * Provide the treasury collateral asset gross value minor operation used by this module
 * and its client applications.
 */
int64_t umi_treasury_collateral_asset_gross_value_minor(const UmiTreasuryCollateralAsset *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->quantity * value->unit_value_minor;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryCollateralAssetArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x2745c47067aed837);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryCollateralAsset *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryCollateralAssetArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryCollateralAsset *)0)->id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiTreasuryCollateralAssetArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryCollateralAsset *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->quantity);
    UmiArchiveWriteSigned(writer, (int64_t)value->unit_value_minor);
}
static void UmiTreasuryCollateralAssetArchiveRead(UmiArchiveReader *reader, UmiTreasuryCollateralAsset *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->kind = (UmiTreasuryCollateralKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->quantity = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->unit_value_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasuryCollateralAssetArchiveValidate(const UmiTreasuryCollateralAsset *value)
{
    return umi_treasury_collateral_asset_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_collateral_asset_archive_encode, umi_treasury_collateral_asset_archive_decode,
    UmiTreasuryCollateralAsset, UmiTreasuryCollateralAssetArchiveSchema, UmiTreasuryCollateralAssetArchiveBound, UmiTreasuryCollateralAssetArchiveWrite, UmiTreasuryCollateralAssetArchiveRead, UmiTreasuryCollateralAssetArchiveValidate)
