/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_delta_package.c
 *
 * PURPOSE:
 *   Focused regression coverage for delta package base/target version and savings validation.
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
#include "umicom/distribution/runtime/delta_package.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/delta_package.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrDeltaPackageTransferEqual(const UmiDrDeltaPackage *a, const UmiDrDeltaPackage *b)
{
    return strcmp(a->id, b->id) == 0 &&
        a->base_version.major == b->base_version.major &&
        a->base_version.minor == b->base_version.minor &&
        a->base_version.patch == b->base_version.patch &&
        a->target_version.major == b->target_version.major &&
        a->target_version.minor == b->target_version.minor &&
        a->target_version.patch == b->target_version.patch &&
        a->full_size == b->full_size &&
        a->delta_size == b->delta_size &&
        strcmp(a->digest, b->digest) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrDeltaPackageTransferTails(UmiDrDeltaPackage *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->digest) + 1U;
        memset(value->digest + used, 0xa5, sizeof(value->digest) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrDeltaPackageTransferMalformed(const UmiDrDeltaPackage *sample)
{
    (void)sample;
    {
        UmiDrDeltaPackage invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_delta_package_valid(&invalid)) ||
            umi_dr_delta_package_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrDeltaPackage invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.digest, 'x', sizeof(invalid.digest));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_delta_package_valid(&invalid)) ||
            umi_dr_delta_package_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated digest was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrDeltaPackageTransferCases, UmiDrDeltaPackage,
    umi_dr_delta_package_archive_encode, umi_dr_delta_package_archive_decode,
    UmiDrDeltaPackageTransferEqual, UmiDrDeltaPackageTransferTails, UmiDrDeltaPackageTransferMalformed)

int main(void) {
    UmiDrDeltaPackage value; umi_dr_delta_package_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"delta")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.digest,sizeof(value.digest),"d")==UMI_STATUS_OK); value.base_version=(UmiDrVersion){1,0,0}; value.target_version=(UmiDrVersion){1,1,0}; value.full_size=100U; value.delta_size=20U; CHECK(umi_dr_delta_package_valid(&value));
    if (UmiDrDeltaPackageTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_delta_package_fingerprint(&value) != 0U);
    return 0;
}
