/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_application_manifest.c
 *
 * PURPOSE:
 *   Focused regression coverage for cross-platform application identity and runtime requirement manifest.
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
#include "umicom/distribution/runtime/application_manifest.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/application_manifest.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrApplicationManifestTransferEqual(const UmiDrApplicationManifest *a, const UmiDrApplicationManifest *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->name, b->name) == 0 &&
        a->version.major == b->version.major &&
        a->version.minor == b->version.minor &&
        a->version.patch == b->version.patch &&
        a->required_capabilities == b->required_capabilities &&
        a->gui == b->gui &&
        a->networked == b->networked;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrApplicationManifestTransferTails(UmiDrApplicationManifest *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrApplicationManifestTransferMalformed(const UmiDrApplicationManifest *sample)
{
    (void)sample;
    {
        UmiDrApplicationManifest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_application_manifest_valid(&invalid)) ||
            umi_dr_application_manifest_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrApplicationManifest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_application_manifest_valid(&invalid)) ||
            umi_dr_application_manifest_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrApplicationManifestTransferCases, UmiDrApplicationManifest,
    umi_dr_application_manifest_archive_encode, umi_dr_application_manifest_archive_decode,
    UmiDrApplicationManifestTransferEqual, UmiDrApplicationManifestTransferTails, UmiDrApplicationManifestTransferMalformed)

int main(void) {
    UmiDrApplicationManifest value; umi_dr_application_manifest_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"studio") == UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.name,sizeof(value.name),"Umicom Studio") == UMI_STATUS_OK); CHECK(umi_dr_application_manifest_valid(&value));
    if (UmiDrApplicationManifestTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_application_manifest_fingerprint(&value) != 0U);
    return 0;
}
