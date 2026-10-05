/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/range_mapping.c
 *
 * PURPOSE:
 *   Map source ranges to destination ranges after edits or diff alignment.
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
#include "umicom/vcs/advanced/range_mapping.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise vcs advanced range mapping from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_range_mapping_init(UmiVcsAdvancedRangeMapping *value)
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
 * Check that vcs advanced range mapping satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_range_mapping_validate(const UmiVcsAdvancedRangeMapping *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->source_count == 0U || value->target_count == 0U || value->confidence_percent > 100U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced range mapping delta operation used by this module and its
 * client applications.
 */
long long umi_vcs_advanced_range_mapping_delta(const UmiVcsAdvancedRangeMapping *value)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_range_mapping_validate(value) != UMI_STATUS_OK) return 0LL;
    return (long long)value->target_start - (long long)value->source_start;
}
/*
 * Provide the vcs advanced range mapping contains source operation used by this module and
 * its client applications.
 */
int umi_vcs_advanced_range_mapping_contains_source(const UmiVcsAdvancedRangeMapping *value,
                                                     size_t line)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_range_mapping_validate(value) != UMI_STATUS_OK) return 0;
    return line >= value->source_start && line < value->source_start + value->source_count;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiVcsAdvancedRangeMappingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x9197562e7184a484);

    return schema;
}
static size_t UmiVcsAdvancedRangeMappingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiVcsAdvancedRangeMappingArchiveWrite(UmiArchiveWriter *writer, const UmiVcsAdvancedRangeMapping *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->source_start);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->source_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->target_start);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->target_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->confidence_percent);
}
static void UmiVcsAdvancedRangeMappingArchiveRead(UmiArchiveReader *reader, UmiVcsAdvancedRangeMapping *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->source_start = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->source_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->target_start = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->target_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->confidence_percent = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiVcsAdvancedRangeMappingArchiveValidate(const UmiVcsAdvancedRangeMapping *value)
{
    return umi_vcs_advanced_range_mapping_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_vcs_advanced_range_mapping_archive_encode, umi_vcs_advanced_range_mapping_archive_decode,
    UmiVcsAdvancedRangeMapping, UmiVcsAdvancedRangeMappingArchiveSchema, UmiVcsAdvancedRangeMappingArchiveBound, UmiVcsAdvancedRangeMappingArchiveWrite, UmiVcsAdvancedRangeMappingArchiveRead, UmiVcsAdvancedRangeMappingArchiveValidate)
