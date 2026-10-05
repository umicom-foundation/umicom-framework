/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_backup_manifest.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the backup manifest enterprise data capability.
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
#include "umicom/data/enterprise/backup_manifest.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/backup_manifest.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataBackupManifestTransferEqual(const UmiDataBackupManifest *a, const UmiDataBackupManifest *b)
{
    return strcmp(a->backup_id, b->backup_id) == 0 &&
        a->created_at == b->created_at &&
        a->schema_fingerprint == b->schema_fingerprint &&
        a->content_fingerprint == b->content_fingerprint &&
        a->bytes_written == b->bytes_written &&
        a->complete == b->complete;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataBackupManifestTransferTails(UmiDataBackupManifest *value)
{
    (void)value;
    {
        size_t used = strlen(value->backup_id) + 1U;
        memset(value->backup_id + used, 0xa5, sizeof(value->backup_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataBackupManifestTransferMalformed(const UmiDataBackupManifest *sample)
{
    (void)sample;
    {
        UmiDataBackupManifest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.backup_id, 'x', sizeof(invalid.backup_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_backup_manifest_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_backup_manifest_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated backup_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataBackupManifestTransferCases, UmiDataBackupManifest,
    umi_data_backup_manifest_archive_encode, umi_data_backup_manifest_archive_decode,
    UmiDataBackupManifestTransferEqual, UmiDataBackupManifestTransferTails, UmiDataBackupManifestTransferMalformed)

int main(void) {
    UmiDataBackupManifest item;
    CHECK(umi_data_backup_manifest_init(&item,"b1",10U,11U,22U,4096U) == UMI_STATUS_OK);
    if (UmiDataBackupManifestTransferCases(&item) != 0) return 1;

    CHECK(item.complete);
    return 0;
}
