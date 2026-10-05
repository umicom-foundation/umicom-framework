/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_debug_package.c
 *
 * PURPOSE:
 *   Focused regression coverage for diagnostic/debug companion package metadata.
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
#include "umicom/distribution/runtime/debug_package.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/debug_package.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrDebugPackageTransferEqual(const UmiDrDebugPackage *a, const UmiDrDebugPackage *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->application_id, b->application_id) == 0 &&
        a->symbols == b->symbols &&
        a->diagnostics == b->diagnostics &&
        a->source_maps == b->source_maps;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrDebugPackageTransferTails(UmiDrDebugPackage *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->application_id) + 1U;
        memset(value->application_id + used, 0xa5, sizeof(value->application_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrDebugPackageTransferMalformed(const UmiDrDebugPackage *sample)
{
    (void)sample;
    {
        UmiDrDebugPackage invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_debug_package_valid(&invalid)) ||
            umi_dr_debug_package_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrDebugPackage invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_debug_package_valid(&invalid)) ||
            umi_dr_debug_package_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrDebugPackageTransferCases, UmiDrDebugPackage,
    umi_dr_debug_package_archive_encode, umi_dr_debug_package_archive_decode,
    UmiDrDebugPackageTransferEqual, UmiDrDebugPackageTransferTails, UmiDrDebugPackageTransferMalformed)

int main(void) {
    UmiDrDebugPackage value; umi_dr_debug_package_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"dbg")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.application_id,sizeof(value.application_id),"app")==UMI_STATUS_OK); value.symbols=true; CHECK(umi_dr_debug_package_valid(&value));
    if (UmiDrDebugPackageTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_debug_package_fingerprint(&value) != 0U);
    return 0;
}
