/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_executable_layout.c
 *
 * PURPOSE:
 *   Focused regression coverage for executable placement and launch-entry validation.
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
#include "umicom/distribution/runtime/executable_layout.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/executable_layout.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrExecutableLayoutTransferEqual(const UmiDrExecutableLayout *a, const UmiDrExecutableLayout *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->entrypoint, b->entrypoint) == 0 &&
        strcmp(a->bin_dir, b->bin_dir) == 0 &&
        a->console == b->console &&
        a->gui == b->gui;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrExecutableLayoutTransferTails(UmiDrExecutableLayout *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->entrypoint) + 1U;
        memset(value->entrypoint + used, 0xa5, sizeof(value->entrypoint) - used);
    }
    {
        size_t used = strlen(value->bin_dir) + 1U;
        memset(value->bin_dir + used, 0xa5, sizeof(value->bin_dir) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrExecutableLayoutTransferMalformed(const UmiDrExecutableLayout *sample)
{
    (void)sample;
    {
        UmiDrExecutableLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_executable_layout_valid(&invalid)) ||
            umi_dr_executable_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrExecutableLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.entrypoint, 'x', sizeof(invalid.entrypoint));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_executable_layout_valid(&invalid)) ||
            umi_dr_executable_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated entrypoint was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrExecutableLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.bin_dir, 'x', sizeof(invalid.bin_dir));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_executable_layout_valid(&invalid)) ||
            umi_dr_executable_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated bin_dir was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrExecutableLayoutTransferCases, UmiDrExecutableLayout,
    umi_dr_executable_layout_archive_encode, umi_dr_executable_layout_archive_decode,
    UmiDrExecutableLayoutTransferEqual, UmiDrExecutableLayoutTransferTails, UmiDrExecutableLayoutTransferMalformed)

int main(void) {
    UmiDrExecutableLayout value; umi_dr_executable_layout_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"exec")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.entrypoint,sizeof(value.entrypoint),"bin/app")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.bin_dir,sizeof(value.bin_dir),"bin")==UMI_STATUS_OK); CHECK(umi_dr_executable_layout_valid(&value));
    if (UmiDrExecutableLayoutTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_executable_layout_fingerprint(&value) != 0U);
    return 0;
}
