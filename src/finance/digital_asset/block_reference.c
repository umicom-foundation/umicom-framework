/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/block_reference.c
 *
 * PURPOSE:
 *   Implement an immutable network block reference used by settlement and audit evidence.
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

#include "umicom/finance/digital_asset/block_reference.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_block_reference_init(UmiDigitalBlockReference *value, const char *network_id, uint64_t height, const char *block_hash)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->network_id.value, sizeof value->network_id.value, network_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->block_hash, sizeof value->block_hash, block_hash);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->height = height;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_block_reference_valid(const UmiDigitalBlockReference *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->network_id.value, '\0', sizeof(value->network_id.value)) == NULL) return 0;
    if (memchr(value->block_hash, '\0', sizeof(value->block_hash)) == NULL) return 0;

    return value != NULL && (umi_digital_asset_text_valid(value->network_id.value) && umi_digital_asset_text_valid(value->block_hash));
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDigitalBlockReferenceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7d3c8e55239a692a);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalBlockReference *)0)->network_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalBlockReference *)0)->block_hash)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDigitalBlockReferenceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDigitalBlockReference *)0)->network_id.value) - 1U +
        8U +
        8U + sizeof(((UmiDigitalBlockReference *)0)->block_hash) - 1U;
}
static void UmiDigitalBlockReferenceArchiveWrite(UmiArchiveWriter *writer, const UmiDigitalBlockReference *value)
{
    UmiArchiveWriteText(writer, value->network_id.value, sizeof(value->network_id.value));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->height);
    UmiArchiveWriteText(writer, value->block_hash, sizeof(value->block_hash));
}
static void UmiDigitalBlockReferenceArchiveRead(UmiArchiveReader *reader, UmiDigitalBlockReference *value)
{
    UmiArchiveReadText(reader, value->network_id.value, sizeof(value->network_id.value));
    value->height = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    UmiArchiveReadText(reader, value->block_hash, sizeof(value->block_hash));
}
static UmiStatus UmiDigitalBlockReferenceArchiveValidate(const UmiDigitalBlockReference *value)
{
    return umi_digital_asset_block_reference_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_digital_asset_block_reference_archive_encode, umi_digital_asset_block_reference_archive_decode,
    UmiDigitalBlockReference, UmiDigitalBlockReferenceArchiveSchema, UmiDigitalBlockReferenceArchiveBound, UmiDigitalBlockReferenceArchiveWrite, UmiDigitalBlockReferenceArchiveRead, UmiDigitalBlockReferenceArchiveValidate)
