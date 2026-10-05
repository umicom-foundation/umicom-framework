/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_data_layout.c
 *
 * PURPOSE:
 *   Focused regression coverage for read-only packaged data and writable application-data separation.
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
#include "umicom/distribution/runtime/data_layout.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/data_layout.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrDataLayoutTransferEqual(const UmiDrDataLayout *a, const UmiDrDataLayout *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->read_only_dir, b->read_only_dir) == 0 &&
        strcmp(a->writable_dir, b->writable_dir) == 0 &&
        a->migrate_legacy == b->migrate_legacy;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrDataLayoutTransferTails(UmiDrDataLayout *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->read_only_dir) + 1U;
        memset(value->read_only_dir + used, 0xa5, sizeof(value->read_only_dir) - used);
    }
    {
        size_t used = strlen(value->writable_dir) + 1U;
        memset(value->writable_dir + used, 0xa5, sizeof(value->writable_dir) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrDataLayoutTransferMalformed(const UmiDrDataLayout *sample)
{
    (void)sample;
    {
        UmiDrDataLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_data_layout_valid(&invalid)) ||
            umi_dr_data_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrDataLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.read_only_dir, 'x', sizeof(invalid.read_only_dir));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_data_layout_valid(&invalid)) ||
            umi_dr_data_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated read_only_dir was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrDataLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.writable_dir, 'x', sizeof(invalid.writable_dir));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_data_layout_valid(&invalid)) ||
            umi_dr_data_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated writable_dir was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrDataLayoutTransferCases, UmiDrDataLayout,
    umi_dr_data_layout_archive_encode, umi_dr_data_layout_archive_decode,
    UmiDrDataLayoutTransferEqual, UmiDrDataLayoutTransferTails, UmiDrDataLayoutTransferMalformed)

int main(void) {
    UmiDrDataLayout value; umi_dr_data_layout_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"data")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.read_only_dir,sizeof(value.read_only_dir),"share")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.writable_dir,sizeof(value.writable_dir),"state")==UMI_STATUS_OK); CHECK(umi_dr_data_layout_valid(&value));
    if (UmiDrDataLayoutTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_data_layout_fingerprint(&value) != 0U);
    return 0;
}
