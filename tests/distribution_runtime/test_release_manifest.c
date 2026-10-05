/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_release_manifest.c
 *
 * PURPOSE:
 *   Focused regression coverage for release identity, channel, platform matrix and artifact summary.
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
#include "umicom/distribution/runtime/release_manifest.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/release_manifest.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrReleaseManifestTransferEqual(const UmiDrReleaseManifest *a, const UmiDrReleaseManifest *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->application_id, b->application_id) == 0 &&
        a->version.major == b->version.major &&
        a->version.minor == b->version.minor &&
        a->version.patch == b->version.patch &&
        a->channel == b->channel &&
        a->artifact_count == b->artifact_count &&
        a->platform_count == b->platform_count &&
        a->fingerprint == b->fingerprint;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrReleaseManifestTransferTails(UmiDrReleaseManifest *value)
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
static int UmiDrReleaseManifestTransferMalformed(const UmiDrReleaseManifest *sample)
{
    (void)sample;
    {
        UmiDrReleaseManifest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_release_manifest_valid(&invalid)) ||
            umi_dr_release_manifest_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrReleaseManifest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_release_manifest_valid(&invalid)) ||
            umi_dr_release_manifest_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrReleaseManifestTransferCases, UmiDrReleaseManifest,
    umi_dr_release_manifest_archive_encode, umi_dr_release_manifest_archive_decode,
    UmiDrReleaseManifestTransferEqual, UmiDrReleaseManifestTransferTails, UmiDrReleaseManifestTransferMalformed)

int main(void) {
    UmiDrReleaseManifest value; umi_dr_release_manifest_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"rel")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.application_id,sizeof(value.application_id),"app")==UMI_STATUS_OK); value.channel=UMI_DR_CHANNEL_STABLE; value.artifact_count=1U; value.platform_count=1U; CHECK(umi_dr_release_manifest_valid(&value));
    if (UmiDrReleaseManifestTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_release_manifest_fingerprint(&value) != 0U);
    return 0;
}
