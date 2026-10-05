/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_sdk_package.c
 *
 * PURPOSE:
 *   Focused regression coverage for developer SDK package metadata and ABI compatibility range.
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
#include "umicom/distribution/runtime/sdk_package.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/sdk_package.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrSdkPackageTransferEqual(const UmiDrSdkPackage *a, const UmiDrSdkPackage *b)
{
    return strcmp(a->id, b->id) == 0 &&
        a->version.major == b->version.major &&
        a->version.minor == b->version.minor &&
        a->version.patch == b->version.patch &&
        a->minimum_abi.major == b->minimum_abi.major &&
        a->minimum_abi.minor == b->minimum_abi.minor &&
        a->minimum_abi.patch == b->minimum_abi.patch &&
        a->maximum_abi.major == b->maximum_abi.major &&
        a->maximum_abi.minor == b->maximum_abi.minor &&
        a->maximum_abi.patch == b->maximum_abi.patch &&
        a->headers == b->headers &&
        a->libraries == b->libraries;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrSdkPackageTransferTails(UmiDrSdkPackage *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrSdkPackageTransferMalformed(const UmiDrSdkPackage *sample)
{
    (void)sample;
    {
        UmiDrSdkPackage invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_sdk_package_valid(&invalid)) ||
            umi_dr_sdk_package_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrSdkPackageTransferCases, UmiDrSdkPackage,
    umi_dr_sdk_package_archive_encode, umi_dr_sdk_package_archive_decode,
    UmiDrSdkPackageTransferEqual, UmiDrSdkPackageTransferTails, UmiDrSdkPackageTransferMalformed)

int main(void) {
    UmiDrSdkPackage value; umi_dr_sdk_package_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"sdk")==UMI_STATUS_OK); value.headers=true; value.maximum_abi=(UmiDrVersion){1,0,0}; CHECK(umi_dr_sdk_package_valid(&value));
    if (UmiDrSdkPackageTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_sdk_package_fingerprint(&value) != 0U);
    return 0;
}
