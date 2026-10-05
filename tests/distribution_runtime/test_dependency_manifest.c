/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_dependency_manifest.c
 *
 * PURPOSE:
 *   Focused regression coverage for package dependency declaration with version and optionality constraints.
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
#include "umicom/distribution/runtime/dependency_manifest.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/dependency_manifest.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrDependencyManifestTransferEqual(const UmiDrDependencyManifest *a, const UmiDrDependencyManifest *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->package_id, b->package_id) == 0 &&
        a->minimum_version.major == b->minimum_version.major &&
        a->minimum_version.minor == b->minimum_version.minor &&
        a->minimum_version.patch == b->minimum_version.patch &&
        a->optional == b->optional;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrDependencyManifestTransferTails(UmiDrDependencyManifest *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->package_id) + 1U;
        memset(value->package_id + used, 0xa5, sizeof(value->package_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrDependencyManifestTransferMalformed(const UmiDrDependencyManifest *sample)
{
    (void)sample;
    {
        UmiDrDependencyManifest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_dependency_manifest_valid(&invalid)) ||
            umi_dr_dependency_manifest_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrDependencyManifest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.package_id, 'x', sizeof(invalid.package_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_dependency_manifest_valid(&invalid)) ||
            umi_dr_dependency_manifest_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated package_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrDependencyManifestTransferCases, UmiDrDependencyManifest,
    umi_dr_dependency_manifest_archive_encode, umi_dr_dependency_manifest_archive_decode,
    UmiDrDependencyManifestTransferEqual, UmiDrDependencyManifestTransferTails, UmiDrDependencyManifestTransferMalformed)

int main(void) {
    UmiDrDependencyManifest value; umi_dr_dependency_manifest_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"dep") == UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.package_id,sizeof(value.package_id),"framework") == UMI_STATUS_OK); CHECK(umi_dr_dependency_manifest_valid(&value));
    if (UmiDrDependencyManifestTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_dependency_manifest_fingerprint(&value) != 0U);
    return 0;
}
