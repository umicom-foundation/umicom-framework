/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_launcher_manifest.c
 *
 * PURPOSE:
 *   Focused regression coverage for launcher executable, arguments and working-directory contract.
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
#include "umicom/distribution/runtime/launcher_manifest.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/distribution/runtime/launcher_manifest.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDrLauncherManifestTransferEqual(const UmiDrLauncherManifest *a, const UmiDrLauncherManifest *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->executable, b->executable) == 0 &&
        strcmp(a->working_directory, b->working_directory) == 0 &&
        strcmp(a->arguments, b->arguments) == 0 &&
        a->single_instance == b->single_instance;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDrLauncherManifestTransferTails(UmiDrLauncherManifest *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->executable) + 1U;
        memset(value->executable + used, 0xa5, sizeof(value->executable) - used);
    }
    {
        size_t used = strlen(value->working_directory) + 1U;
        memset(value->working_directory + used, 0xa5, sizeof(value->working_directory) - used);
    }
    {
        size_t used = strlen(value->arguments) + 1U;
        memset(value->arguments + used, 0xa5, sizeof(value->arguments) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDrLauncherManifestTransferMalformed(const UmiDrLauncherManifest *sample)
{
    (void)sample;
    {
        UmiDrLauncherManifest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_launcher_manifest_valid(&invalid)) ||
            umi_dr_launcher_manifest_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrLauncherManifest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.executable, 'x', sizeof(invalid.executable));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_launcher_manifest_valid(&invalid)) ||
            umi_dr_launcher_manifest_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated executable was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrLauncherManifest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.working_directory, 'x', sizeof(invalid.working_directory));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_launcher_manifest_valid(&invalid)) ||
            umi_dr_launcher_manifest_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated working_directory was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDrLauncherManifest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.arguments, 'x', sizeof(invalid.arguments));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_dr_launcher_manifest_valid(&invalid)) ||
            umi_dr_launcher_manifest_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated arguments was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDrLauncherManifestTransferCases, UmiDrLauncherManifest,
    umi_dr_launcher_manifest_archive_encode, umi_dr_launcher_manifest_archive_decode,
    UmiDrLauncherManifestTransferEqual, UmiDrLauncherManifestTransferTails, UmiDrLauncherManifestTransferMalformed)

int main(void) {
    UmiDrLauncherManifest value; umi_dr_launcher_manifest_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"default")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.executable,sizeof(value.executable),"bin/app")==UMI_STATUS_OK); CHECK(umi_dr_launcher_manifest_valid(&value));
    if (UmiDrLauncherManifestTransferCases(&value) != 0) return 1;
 CHECK(umi_dr_launcher_manifest_fingerprint(&value) != 0U);
    return 0;
}
