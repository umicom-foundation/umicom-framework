/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_migration_checkpoint.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the migration checkpoint enterprise data capability.
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
#include "umicom/data/enterprise/migration_checkpoint.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/migration_checkpoint.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataMigrationCheckpointTransferEqual(const UmiDataMigrationCheckpoint *a, const UmiDataMigrationCheckpoint *b)
{
    return strcmp(a->checkpoint_id, b->checkpoint_id) == 0 &&
        strcmp(a->migration_id, b->migration_id) == 0 &&
        a->completed_steps == b->completed_steps &&
        a->source_fingerprint == b->source_fingerprint &&
        a->current_fingerprint == b->current_fingerprint &&
        a->committed == b->committed;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataMigrationCheckpointTransferTails(UmiDataMigrationCheckpoint *value)
{
    (void)value;
    {
        size_t used = strlen(value->checkpoint_id) + 1U;
        memset(value->checkpoint_id + used, 0xa5, sizeof(value->checkpoint_id) - used);
    }
    {
        size_t used = strlen(value->migration_id) + 1U;
        memset(value->migration_id + used, 0xa5, sizeof(value->migration_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataMigrationCheckpointTransferMalformed(const UmiDataMigrationCheckpoint *sample)
{
    (void)sample;
    {
        UmiDataMigrationCheckpoint invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.checkpoint_id, 'x', sizeof(invalid.checkpoint_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_migration_checkpoint_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_migration_checkpoint_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated checkpoint_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataMigrationCheckpoint invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.migration_id, 'x', sizeof(invalid.migration_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_migration_checkpoint_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_migration_checkpoint_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated migration_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataMigrationCheckpointTransferCases, UmiDataMigrationCheckpoint,
    umi_data_migration_checkpoint_archive_encode, umi_data_migration_checkpoint_archive_decode,
    UmiDataMigrationCheckpointTransferEqual, UmiDataMigrationCheckpointTransferTails, UmiDataMigrationCheckpointTransferMalformed)

int main(void) {
    UmiDataMigrationCheckpoint item;
    CHECK(umi_data_migration_checkpoint_init(&item,"cp1","mig1",2U,11U,22U) == UMI_STATUS_OK);
    if (UmiDataMigrationCheckpointTransferCases(&item) != 0) return 1;

    CHECK(item.completed_steps==2U);
    return 0;
}
