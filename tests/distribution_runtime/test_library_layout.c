/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_library_layout.c
 *
 * PURPOSE:
 *   Focused regression coverage for shared/private runtime library placement policy.
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
#include "umicom/distribution/runtime/library_layout.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/library_layout.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrLibraryLayoutTransferEqual(const UmiDrLibraryLayout *a, const UmiDrLibraryLayout *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->private_dir, b->private_dir) == 0 &&
        strcmp(a->system_hint, b->system_hint) == 0 &&
        a->search_relative == b->search_relative;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrLibraryLayoutTransferTails(UmiDrLibraryLayout *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->private_dir) + 1U;
        memset(value->private_dir + used, 0xa5, sizeof(value->private_dir) - used);
    }
    {
        size_t used = strlen(value->system_hint) + 1U;
        memset(value->system_hint + used, 0xa5, sizeof(value->system_hint) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrLibraryLayoutTransferMalformed(const UmiDrLibraryLayout *sample)
{
    (void)sample;
    {
        UmiDrLibraryLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_library_layout_valid(&invalid)) ||
            umi_dr_library_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrLibraryLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.private_dir, 'x', sizeof(invalid.private_dir));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_library_layout_valid(&invalid)) ||
            umi_dr_library_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated private_dir was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrLibraryLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.system_hint, 'x', sizeof(invalid.system_hint));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_library_layout_valid(&invalid)) ||
            umi_dr_library_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated system_hint was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrLibraryLayoutTransferCases, UmiDrLibraryLayout,
    umi_dr_library_layout_archive_encode, umi_dr_library_layout_archive_decode,
    UmiDrLibraryLayoutTransferEqual, UmiDrLibraryLayoutTransferTails, UmiDrLibraryLayoutTransferMalformed)

int main(void) {
    UmiDrLibraryLayout value; umi_dr_library_layout_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"libs")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.private_dir,sizeof(value.private_dir),"lib")==UMI_STATUS_OK); value.search_relative=true; CHECK(umi_dr_library_layout_valid(&value));
    if (UmiDrLibraryLayoutTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_library_layout_fingerprint(&value) != 0U);
    return 0;
}
