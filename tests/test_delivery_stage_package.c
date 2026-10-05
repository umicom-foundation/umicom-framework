/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_stage_package.c
 *
 * PURPOSE:
 *   Verify the delivery-platform behaviour exercised by this focused test.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This small executable uses assertions so a failure points directly at one delivery contract.
 */

/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include "umicom/delivery/stage.h"
#include "umicom/delivery/directory_package.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "value_archive/transfer_cases.h"

#include "umicom/delivery/package.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPackageSpecTransferEqual(const UmiPackageSpec *a, const UmiPackageSpec *b)
{
    return strcmp(a->package_id, b->package_id) == 0 &&
        a->format == b->format &&
        strcmp(a->staging_root, b->staging_root) == 0 &&
        strcmp(a->output_path, b->output_path) == 0 &&
        a->include_symbols == b->include_symbols;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPackageSpecTransferTails(UmiPackageSpec *value)
{
    (void)value;
    {
        size_t used = strlen(value->package_id) + 1U;
        memset(value->package_id + used, 0xa5, sizeof(value->package_id) - used);
    }
    {
        size_t used = strlen(value->staging_root) + 1U;
        memset(value->staging_root + used, 0xa5, sizeof(value->staging_root) - used);
    }
    {
        size_t used = strlen(value->output_path) + 1U;
        memset(value->output_path + used, 0xa5, sizeof(value->output_path) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPackageSpecTransferMalformed(const UmiPackageSpec *sample)
{
    (void)sample;
    {
        UmiPackageSpec invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.package_id, 'x', sizeof(invalid.package_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_package_spec_validate(&invalid) != UMI_STATUS_OK) ||
            umi_package_spec_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated package_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPackageSpec invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.staging_root, 'x', sizeof(invalid.staging_root));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_package_spec_validate(&invalid) != UMI_STATUS_OK) ||
            umi_package_spec_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated staging_root was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPackageSpec invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.output_path, 'x', sizeof(invalid.output_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_package_spec_validate(&invalid) != UMI_STATUS_OK) ||
            umi_package_spec_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated output_path was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPackageSpecTransferCases, UmiPackageSpec,
    umi_package_spec_archive_encode, umi_package_spec_archive_decode,
    UmiPackageSpecTransferEqual, UmiPackageSpecTransferTails, UmiPackageSpecTransferMalformed)

int main(void) {
    UmiDeliveryStageRecord stage;
    UmiPackageSpec spec;
    UmiPackageProvider provider;
    UmiPackageResult result;
    assert(umi_delivery_stage_init(&stage, "stage/root") == UMI_STATUS_OK);
    umi_delivery_stage_add_file(&stage, 100U);
    umi_delivery_stage_complete(&stage);
    assert(stage.complete && stage.total_bytes == 100U);
    assert(umi_package_spec_init(&spec, "portable", UMI_PACKAGE_DIRECTORY, "stage/root", "dist/studio") == UMI_STATUS_OK);
    if (UmiPackageSpecTransferCases(&spec) != 0) return 1;

    assert(umi_directory_package_provider(&provider) == UMI_STATUS_OK);
    assert(umi_package_provider_create(&provider, &spec, &result) == UMI_STATUS_OK);
    assert(result.succeeded);
    return 0;
}
