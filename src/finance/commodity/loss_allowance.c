/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/loss_allowance.c
 *
 * PURPOSE:
 *   Implement permitted physical loss as basis points of shipped quantity.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/commodity/loss_allowance.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_loss_allowance_init(UmiCommodityLossAllowance *value, const char *contract_id, int32_t basis_points)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || basis_points < 0 || basis_points > 10000) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->contract_id.value, sizeof value->contract_id.value, contract_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->basis_points = basis_points;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_loss_allowance_valid(const UmiCommodityLossAllowance *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->contract_id.value, '\0', sizeof(value->contract_id.value)) == NULL) return 0;

    return value != NULL && (umi_commodity_text_valid(value->contract_id.value) && value->basis_points >= 0 && value->basis_points <= 10000 && value->active);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCommodityLossAllowanceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x24db930d3c6ec40b);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityLossAllowance *)0)->contract_id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCommodityLossAllowanceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCommodityLossAllowance *)0)->contract_id.value) - 1U +
        8U +
        8U;
}
static void UmiCommodityLossAllowanceArchiveWrite(UmiArchiveWriter *writer, const UmiCommodityLossAllowance *value)
{
    UmiArchiveWriteText(writer, value->contract_id.value, sizeof(value->contract_id.value));
    UmiArchiveWriteSigned(writer, (int64_t)value->basis_points);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiCommodityLossAllowanceArchiveRead(UmiArchiveReader *reader, UmiCommodityLossAllowance *value)
{
    UmiArchiveReadText(reader, value->contract_id.value, sizeof(value->contract_id.value));
    value->basis_points = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiCommodityLossAllowanceArchiveValidate(const UmiCommodityLossAllowance *value)
{
    return umi_commodity_loss_allowance_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_commodity_loss_allowance_archive_encode, umi_commodity_loss_allowance_archive_decode,
    UmiCommodityLossAllowance, UmiCommodityLossAllowanceArchiveSchema, UmiCommodityLossAllowanceArchiveBound, UmiCommodityLossAllowanceArchiveWrite, UmiCommodityLossAllowanceArchiveRead, UmiCommodityLossAllowanceArchiveValidate)
