/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_application_bundle.c
 *
 * PURPOSE:
 *   Focused regression coverage for application bundle metadata, selected variant and immutable content fingerprint.
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
#include "umicom/distribution/runtime/application_bundle.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/application_bundle.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrApplicationBundleTransferEqual(const UmiDrApplicationBundle *a, const UmiDrApplicationBundle *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->application_id, b->application_id) == 0 &&
        strcmp(a->variant_id, b->variant_id) == 0 &&
        a->version.major == b->version.major &&
        a->version.minor == b->version.minor &&
        a->version.patch == b->version.patch &&
        a->content_fingerprint == b->content_fingerprint &&
        a->file_count == b->file_count;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrApplicationBundleTransferTails(UmiDrApplicationBundle *value)
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
    {
        size_t used = strlen(value->variant_id) + 1U;
        memset(value->variant_id + used, 0xa5, sizeof(value->variant_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrApplicationBundleTransferMalformed(const UmiDrApplicationBundle *sample)
{
    (void)sample;
    {
        UmiDrApplicationBundle invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_application_bundle_valid(&invalid)) ||
            umi_dr_application_bundle_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrApplicationBundle invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_application_bundle_valid(&invalid)) ||
            umi_dr_application_bundle_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrApplicationBundle invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.variant_id, 'x', sizeof(invalid.variant_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_application_bundle_valid(&invalid)) ||
            umi_dr_application_bundle_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated variant_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrApplicationBundleTransferCases, UmiDrApplicationBundle,
    umi_dr_application_bundle_archive_encode, umi_dr_application_bundle_archive_decode,
    UmiDrApplicationBundleTransferEqual, UmiDrApplicationBundleTransferTails, UmiDrApplicationBundleTransferMalformed)

int main(void) {
    UmiDrApplicationBundle value; umi_dr_application_bundle_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"bundle") == UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.application_id,sizeof(value.application_id),"studio") == UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.variant_id,sizeof(value.variant_id),"win-x64") == UMI_STATUS_OK); value.file_count=1U; CHECK(umi_dr_application_bundle_valid(&value));
    if (UmiDrApplicationBundleTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_application_bundle_fingerprint(&value) != 0U);
    return 0;
}
