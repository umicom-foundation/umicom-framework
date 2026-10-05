/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_bundle_layout.c
 *
 * PURPOSE:
 *   Focused regression coverage for portable application bundle directory layout validation.
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
#include "umicom/distribution/runtime/bundle_layout.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/bundle_layout.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrBundleLayoutTransferEqual(const UmiDrBundleLayout *a, const UmiDrBundleLayout *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->bin_dir, b->bin_dir) == 0 &&
        strcmp(a->lib_dir, b->lib_dir) == 0 &&
        strcmp(a->share_dir, b->share_dir) == 0 &&
        strcmp(a->state_dir, b->state_dir) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrBundleLayoutTransferTails(UmiDrBundleLayout *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->bin_dir) + 1U;
        memset(value->bin_dir + used, 0xa5, sizeof(value->bin_dir) - used);
    }
    {
        size_t used = strlen(value->lib_dir) + 1U;
        memset(value->lib_dir + used, 0xa5, sizeof(value->lib_dir) - used);
    }
    {
        size_t used = strlen(value->share_dir) + 1U;
        memset(value->share_dir + used, 0xa5, sizeof(value->share_dir) - used);
    }
    {
        size_t used = strlen(value->state_dir) + 1U;
        memset(value->state_dir + used, 0xa5, sizeof(value->state_dir) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrBundleLayoutTransferMalformed(const UmiDrBundleLayout *sample)
{
    (void)sample;
    {
        UmiDrBundleLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_bundle_layout_valid(&invalid)) ||
            umi_dr_bundle_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrBundleLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.bin_dir, 'x', sizeof(invalid.bin_dir));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_bundle_layout_valid(&invalid)) ||
            umi_dr_bundle_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated bin_dir was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrBundleLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.lib_dir, 'x', sizeof(invalid.lib_dir));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_bundle_layout_valid(&invalid)) ||
            umi_dr_bundle_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated lib_dir was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrBundleLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.share_dir, 'x', sizeof(invalid.share_dir));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_bundle_layout_valid(&invalid)) ||
            umi_dr_bundle_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated share_dir was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrBundleLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.state_dir, 'x', sizeof(invalid.state_dir));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_bundle_layout_valid(&invalid)) ||
            umi_dr_bundle_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated state_dir was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrBundleLayoutTransferCases, UmiDrBundleLayout,
    umi_dr_bundle_layout_archive_encode, umi_dr_bundle_layout_archive_decode,
    UmiDrBundleLayoutTransferEqual, UmiDrBundleLayoutTransferTails, UmiDrBundleLayoutTransferMalformed)

int main(void) {
    UmiDrBundleLayout value; umi_dr_bundle_layout_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"standard")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.bin_dir,sizeof(value.bin_dir),"bin")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.lib_dir,sizeof(value.lib_dir),"lib")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.share_dir,sizeof(value.share_dir),"share")==UMI_STATUS_OK); CHECK(umi_dr_bundle_layout_valid(&value));
    if (UmiDrBundleLayoutTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_bundle_layout_fingerprint(&value) != 0U);
    return 0;
}
