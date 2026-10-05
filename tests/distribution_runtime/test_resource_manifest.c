/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_resource_manifest.c
 *
 * PURPOSE:
 *   Focused regression coverage for resource-pack identity, locale, scale and content metadata.
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
#include "umicom/distribution/runtime/resource_manifest.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/resource_manifest.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrResourceManifestTransferEqual(const UmiDrResourceManifest *a, const UmiDrResourceManifest *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->locale, b->locale) == 0 &&
        a->scale_percent == b->scale_percent &&
        a->size_bytes == b->size_bytes &&
        strcmp(a->digest, b->digest) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrResourceManifestTransferTails(UmiDrResourceManifest *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->locale) + 1U;
        memset(value->locale + used, 0xa5, sizeof(value->locale) - used);
    }
    {
        size_t used = strlen(value->digest) + 1U;
        memset(value->digest + used, 0xa5, sizeof(value->digest) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrResourceManifestTransferMalformed(const UmiDrResourceManifest *sample)
{
    (void)sample;
    {
        UmiDrResourceManifest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_resource_manifest_valid(&invalid)) ||
            umi_dr_resource_manifest_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrResourceManifest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.locale, 'x', sizeof(invalid.locale));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_resource_manifest_valid(&invalid)) ||
            umi_dr_resource_manifest_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated locale was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrResourceManifest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.digest, 'x', sizeof(invalid.digest));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_resource_manifest_valid(&invalid)) ||
            umi_dr_resource_manifest_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated digest was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrResourceManifestTransferCases, UmiDrResourceManifest,
    umi_dr_resource_manifest_archive_encode, umi_dr_resource_manifest_archive_decode,
    UmiDrResourceManifestTransferEqual, UmiDrResourceManifestTransferTails, UmiDrResourceManifestTransferMalformed)

int main(void) {
    UmiDrResourceManifest value; umi_dr_resource_manifest_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"base")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.digest,sizeof(value.digest),"d")==UMI_STATUS_OK); CHECK(umi_dr_resource_manifest_valid(&value));
    if (UmiDrResourceManifestTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_resource_manifest_fingerprint(&value) != 0U);
    return 0;
}
