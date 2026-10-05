/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_source_package.c
 *
 * PURPOSE:
 *   Focused regression coverage for source distribution metadata, licence and reproducibility flags.
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
#include "umicom/distribution/runtime/source_package.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/source_package.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrSourcePackageTransferEqual(const UmiDrSourcePackage *a, const UmiDrSourcePackage *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->revision, b->revision) == 0 &&
        strcmp(a->licence, b->licence) == 0 &&
        a->complete == b->complete &&
        a->reproducible == b->reproducible;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrSourcePackageTransferTails(UmiDrSourcePackage *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->revision) + 1U;
        memset(value->revision + used, 0xa5, sizeof(value->revision) - used);
    }
    {
        size_t used = strlen(value->licence) + 1U;
        memset(value->licence + used, 0xa5, sizeof(value->licence) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrSourcePackageTransferMalformed(const UmiDrSourcePackage *sample)
{
    (void)sample;
    {
        UmiDrSourcePackage invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_source_package_valid(&invalid)) ||
            umi_dr_source_package_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrSourcePackage invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.revision, 'x', sizeof(invalid.revision));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_source_package_valid(&invalid)) ||
            umi_dr_source_package_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated revision was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrSourcePackage invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.licence, 'x', sizeof(invalid.licence));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_source_package_valid(&invalid)) ||
            umi_dr_source_package_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated licence was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrSourcePackageTransferCases, UmiDrSourcePackage,
    umi_dr_source_package_archive_encode, umi_dr_source_package_archive_decode,
    UmiDrSourcePackageTransferEqual, UmiDrSourcePackageTransferTails, UmiDrSourcePackageTransferMalformed)

int main(void) {
    UmiDrSourcePackage value; umi_dr_source_package_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"src")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.revision,sizeof(value.revision),"abc")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.licence,sizeof(value.licence),"MIT")==UMI_STATUS_OK); CHECK(umi_dr_source_package_valid(&value));
    if (UmiDrSourcePackageTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_source_package_fingerprint(&value) != 0U);
    return 0;
}
