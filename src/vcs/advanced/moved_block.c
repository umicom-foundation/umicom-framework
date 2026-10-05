/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/moved_block.c
 *
 * PURPOSE:
 *   Capture identical or near-identical blocks moved within a document.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/moved_block.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise vcs advanced moved block from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_moved_block_init(UmiVcsAdvancedMovedBlock *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

/*
 * Check that vcs advanced moved block satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_vcs_advanced_moved_block_validate(const UmiVcsAdvancedMovedBlock *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->line_count == 0U || value->confidence_percent > 100U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced moved block is significant operation used by this module and
 * its client applications.
 */
int umi_vcs_advanced_moved_block_is_significant(const UmiVcsAdvancedMovedBlock *value,
                                                   size_t minimum_lines,
                                                   uint32_t minimum_confidence)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_moved_block_validate(value) != UMI_STATUS_OK) return 0;
    return value->line_count >= minimum_lines &&
           value->confidence_percent >= minimum_confidence &&
           value->old_start != value->new_start;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiVcsAdvancedMovedBlockArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x75f6f2ec1c0315de);

    return schema;
}
static size_t UmiVcsAdvancedMovedBlockArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiVcsAdvancedMovedBlockArchiveWrite(UmiArchiveWriter *writer, const UmiVcsAdvancedMovedBlock *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->old_start);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->new_start);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->line_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->fingerprint);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->confidence_percent);
}
static void UmiVcsAdvancedMovedBlockArchiveRead(UmiArchiveReader *reader, UmiVcsAdvancedMovedBlock *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->old_start = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->new_start = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->line_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->fingerprint = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->confidence_percent = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiVcsAdvancedMovedBlockArchiveValidate(const UmiVcsAdvancedMovedBlock *value)
{
    return umi_vcs_advanced_moved_block_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_vcs_advanced_moved_block_archive_encode, umi_vcs_advanced_moved_block_archive_decode,
    UmiVcsAdvancedMovedBlock, UmiVcsAdvancedMovedBlockArchiveSchema, UmiVcsAdvancedMovedBlockArchiveBound, UmiVcsAdvancedMovedBlockArchiveWrite, UmiVcsAdvancedMovedBlockArchiveRead, UmiVcsAdvancedMovedBlockArchiveValidate)
