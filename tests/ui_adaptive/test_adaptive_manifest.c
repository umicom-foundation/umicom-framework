/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_adaptive_manifest.c
 *
 * PURPOSE:
 *   Validate declare application-wide adaptive shell capabilities and renderer coverage.
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
#include "umicom/ui/adaptive/adaptive_manifest.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/adaptive/adaptive_manifest.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAdaptiveManifestTransferEqual(const UmiAdaptiveManifest *a, const UmiAdaptiveManifest *b)
{
    return strcmp(a->application_id, b->application_id) == 0 &&
        strcmp(a->shell_profile_id, b->shell_profile_id) == 0 &&
        a->renderer_mask == b->renderer_mask &&
        a->breakpoint_count == b->breakpoint_count &&
        a->supports_orientation_change == b->supports_orientation_change &&
        a->supports_multi_window == b->supports_multi_window;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAdaptiveManifestTransferTails(UmiAdaptiveManifest *value)
{
    (void)value;
    {
        size_t used = strlen(value->application_id) + 1U;
        memset(value->application_id + used, 0xa5, sizeof(value->application_id) - used);
    }
    {
        size_t used = strlen(value->shell_profile_id) + 1U;
        memset(value->shell_profile_id + used, 0xa5, sizeof(value->shell_profile_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAdaptiveManifestTransferMalformed(const UmiAdaptiveManifest *sample)
{
    (void)sample;
    {
        UmiAdaptiveManifest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_adaptive_manifest_valid(&invalid)) ||
            umi_adaptive_manifest_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAdaptiveManifest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.shell_profile_id, 'x', sizeof(invalid.shell_profile_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_adaptive_manifest_valid(&invalid)) ||
            umi_adaptive_manifest_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated shell_profile_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAdaptiveManifestTransferCases, UmiAdaptiveManifest,
    umi_adaptive_manifest_archive_encode, umi_adaptive_manifest_archive_decode,
    UmiAdaptiveManifestTransferEqual, UmiAdaptiveManifestTransferTails, UmiAdaptiveManifestTransferMalformed)

int main(void)
{
    UmiAdaptiveManifest manifest;
    CHECK(umi_adaptive_manifest_init(&manifest,"studio","studio.shell")==UMI_STATUS_OK);
    CHECK(umi_adaptive_manifest_valid(&manifest));
    if (UmiAdaptiveManifestTransferCases(&manifest) != 0) return 1;

    return 0;
}
