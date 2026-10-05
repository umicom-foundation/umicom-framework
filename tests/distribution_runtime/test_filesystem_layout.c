/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_filesystem_layout.c
 *
 * PURPOSE:
 *   Focused regression coverage for canonical install-root, bin, lib, share and writable-state layout.
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
#include "umicom/distribution/runtime/filesystem_layout.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/filesystem_layout.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrFilesystemLayoutTransferEqual(const UmiDrFilesystemLayout *a, const UmiDrFilesystemLayout *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->root, b->root) == 0 &&
        strcmp(a->bin, b->bin) == 0 &&
        strcmp(a->lib, b->lib) == 0 &&
        strcmp(a->share, b->share) == 0 &&
        strcmp(a->state, b->state) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrFilesystemLayoutTransferTails(UmiDrFilesystemLayout *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->root) + 1U;
        memset(value->root + used, 0xa5, sizeof(value->root) - used);
    }
    {
        size_t used = strlen(value->bin) + 1U;
        memset(value->bin + used, 0xa5, sizeof(value->bin) - used);
    }
    {
        size_t used = strlen(value->lib) + 1U;
        memset(value->lib + used, 0xa5, sizeof(value->lib) - used);
    }
    {
        size_t used = strlen(value->share) + 1U;
        memset(value->share + used, 0xa5, sizeof(value->share) - used);
    }
    {
        size_t used = strlen(value->state) + 1U;
        memset(value->state + used, 0xa5, sizeof(value->state) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrFilesystemLayoutTransferMalformed(const UmiDrFilesystemLayout *sample)
{
    (void)sample;
    {
        UmiDrFilesystemLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_filesystem_layout_valid(&invalid)) ||
            umi_dr_filesystem_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrFilesystemLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.root, 'x', sizeof(invalid.root));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_filesystem_layout_valid(&invalid)) ||
            umi_dr_filesystem_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated root was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrFilesystemLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.bin, 'x', sizeof(invalid.bin));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_filesystem_layout_valid(&invalid)) ||
            umi_dr_filesystem_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated bin was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrFilesystemLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.lib, 'x', sizeof(invalid.lib));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_filesystem_layout_valid(&invalid)) ||
            umi_dr_filesystem_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated lib was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrFilesystemLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.share, 'x', sizeof(invalid.share));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_filesystem_layout_valid(&invalid)) ||
            umi_dr_filesystem_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated share was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrFilesystemLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.state, 'x', sizeof(invalid.state));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_filesystem_layout_valid(&invalid)) ||
            umi_dr_filesystem_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated state was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrFilesystemLayoutTransferCases, UmiDrFilesystemLayout,
    umi_dr_filesystem_layout_archive_encode, umi_dr_filesystem_layout_archive_decode,
    UmiDrFilesystemLayoutTransferEqual, UmiDrFilesystemLayoutTransferTails, UmiDrFilesystemLayoutTransferMalformed)

int main(void) {
    UmiDrFilesystemLayout value; umi_dr_filesystem_layout_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"portable")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.root,sizeof(value.root),".")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.bin,sizeof(value.bin),"bin")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.lib,sizeof(value.lib),"lib")==UMI_STATUS_OK); CHECK(umi_dr_filesystem_layout_valid(&value));
    if (UmiDrFilesystemLayoutTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_filesystem_layout_fingerprint(&value) != 0U);
    return 0;
}
