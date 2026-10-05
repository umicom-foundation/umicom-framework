/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/commodity_snapshot.c
 *
 * PURPOSE:
 *   Capture bounded aggregate physical-contract, inventory, shipment and nomination evidence.
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

#include "umicom/finance/commodity/commodity_snapshot.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise snapshot evidence without taking ownership of domain books. */
void umi_commodity_commodity_snapshot_init(UmiCommoditySnapshot *value, int64_t captured_time_ms)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value != NULL) {
        memset(value, 0, sizeof *value);
        value->captured_time_ms = captured_time_ms;
        value->revision = 1U;
    }
}

/* Snapshot validity is intentionally independent of whether inventories are empty. */
bool umi_commodity_commodity_snapshot_valid(const UmiCommoditySnapshot *value)
{
    return value != NULL && value->captured_time_ms >= 0 && value->revision > 0U;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCommoditySnapshotArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x829713cebd1d7c8b);

    return schema;
}
static size_t UmiCommoditySnapshotArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiCommoditySnapshotArchiveWrite(UmiArchiveWriter *writer, const UmiCommoditySnapshot *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->commodity_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->contract_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->inventory_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->shipment_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->nomination_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->captured_time_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiCommoditySnapshotArchiveRead(UmiArchiveReader *reader, UmiCommoditySnapshot *value)
{
    value->commodity_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->contract_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->inventory_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->shipment_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->nomination_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->captured_time_ms = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiCommoditySnapshotArchiveValidate(const UmiCommoditySnapshot *value)
{
    return umi_commodity_commodity_snapshot_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_commodity_commodity_snapshot_archive_encode, umi_commodity_commodity_snapshot_archive_decode,
    UmiCommoditySnapshot, UmiCommoditySnapshotArchiveSchema, UmiCommoditySnapshotArchiveBound, UmiCommoditySnapshotArchiveWrite, UmiCommoditySnapshotArchiveRead, UmiCommoditySnapshotArchiveValidate)
