/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_directory_filter.c
 *
 * PURPOSE:
 *   Validate define deterministic inclusion policy for large directory comparisons.
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
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/vcs/advanced/directory_filter.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/vcs/advanced/directory_filter.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiVcsAdvancedDirectoryFilterTransferEqual(const UmiVcsAdvancedDirectoryFilter *a, const UmiVcsAdvancedDirectoryFilter *b)
{
    return a->struct_size == b->struct_size &&
        a->api_version == b->api_version &&
        strcmp(a->extension, b->extension) == 0 &&
        strcmp(a->path_prefix, b->path_prefix) == 0 &&
        a->maximum_size_bytes == b->maximum_size_bytes &&
        a->include_hidden == b->include_hidden &&
        a->include_directories == b->include_directories &&
        a->include_binary == b->include_binary;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiVcsAdvancedDirectoryFilterTransferTails(UmiVcsAdvancedDirectoryFilter *value)
{
    (void)value;
    {
        size_t used = strlen(value->extension) + 1U;
        memset(value->extension + used, 0xa5, sizeof(value->extension) - used);
    }
    {
        size_t used = strlen(value->path_prefix) + 1U;
        memset(value->path_prefix + used, 0xa5, sizeof(value->path_prefix) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiVcsAdvancedDirectoryFilterTransferMalformed(const UmiVcsAdvancedDirectoryFilter *sample)
{
    (void)sample;
    {
        UmiVcsAdvancedDirectoryFilter invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.extension, 'x', sizeof(invalid.extension));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_vcs_advanced_directory_filter_validate(&invalid) != UMI_STATUS_OK) ||
            umi_vcs_advanced_directory_filter_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated extension was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiVcsAdvancedDirectoryFilter invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.path_prefix, 'x', sizeof(invalid.path_prefix));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_vcs_advanced_directory_filter_validate(&invalid) != UMI_STATUS_OK) ||
            umi_vcs_advanced_directory_filter_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated path_prefix was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiVcsAdvancedDirectoryFilterTransferCases, UmiVcsAdvancedDirectoryFilter,
    umi_vcs_advanced_directory_filter_archive_encode, umi_vcs_advanced_directory_filter_archive_decode,
    UmiVcsAdvancedDirectoryFilterTransferEqual, UmiVcsAdvancedDirectoryFilterTransferTails, UmiVcsAdvancedDirectoryFilterTransferMalformed)

int main(void)
{
    UmiVcsAdvancedDirectoryFilter value;
    umi_vcs_advanced_directory_filter_init(&value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_directory_filter_validate(&value) != UMI_STATUS_OK) return 2;
    if (UmiVcsAdvancedDirectoryFilterTransferCases(&value) != 0) return 1;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_vcs_advanced_directory_filter_accept(&value, "src/a.c", 12U, 0, 0, 0)) return 3;
    return 0;
}
