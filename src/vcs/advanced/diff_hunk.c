/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/diff_hunk.c
 *
 * PURPOSE:
 *   Implement normalized change blocks for navigation and partial operations.
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
#include "umicom/vcs/advanced/diff_hunk.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise vcs advanced diff hunk from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_diff_hunk_init(UmiVcsAdvancedDiffHunk *value)
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
 * Check that vcs advanced diff hunk satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_vcs_advanced_diff_hunk_validate(const UmiVcsAdvancedDiffHunk *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->old_count == 0U && value->new_count == 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced diff hunk set counts operation used by this module and its
 * client applications.
 */
void umi_vcs_advanced_diff_hunk_set_counts(UmiVcsAdvancedDiffHunk *value,
                                             size_t added,
                                             size_t deleted,
                                             size_t modified)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    value->added_lines = added;
    value->deleted_lines = deleted;
    value->modified_lines = modified;
}
/*
 * Return the number of records represented by vcs advanced diff hunk change without
 * changing their state.
 */
size_t umi_vcs_advanced_diff_hunk_change_count(const UmiVcsAdvancedDiffHunk *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return 0U;
    return value->added_lines + value->deleted_lines + value->modified_lines;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiVcsAdvancedDiffHunkArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd7747e22feab1381);

    return schema;
}
static size_t UmiVcsAdvancedDiffHunkArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiVcsAdvancedDiffHunkArchiveWrite(UmiArchiveWriter *writer, const UmiVcsAdvancedDiffHunk *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->old_start);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->old_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->new_start);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->new_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->added_lines);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->deleted_lines);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->modified_lines);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->fingerprint);
}
static void UmiVcsAdvancedDiffHunkArchiveRead(UmiArchiveReader *reader, UmiVcsAdvancedDiffHunk *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->old_start = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->old_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->new_start = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->new_count = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->added_lines = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->deleted_lines = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->modified_lines = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->fingerprint = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiVcsAdvancedDiffHunkArchiveValidate(const UmiVcsAdvancedDiffHunk *value)
{
    return umi_vcs_advanced_diff_hunk_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_vcs_advanced_diff_hunk_archive_encode, umi_vcs_advanced_diff_hunk_archive_decode,
    UmiVcsAdvancedDiffHunk, UmiVcsAdvancedDiffHunkArchiveSchema, UmiVcsAdvancedDiffHunkArchiveBound, UmiVcsAdvancedDiffHunkArchiveWrite, UmiVcsAdvancedDiffHunkArchiveRead, UmiVcsAdvancedDiffHunkArchiveValidate)
