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
#include "../value_archive/construction_cases.h"
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

/* Exercise the caller-workspace API using this owner\'s real fixture and field comparator. */
#include "../value_archive/workspace_cases.h"
UMI_TEST_VALUE_WORKSPACE(UmiDataMigrationCheckpointTransferCasesWithWorkspace, UmiDataMigrationCheckpoint,
    umi_data_migration_checkpoint_archive_encode, umi_data_migration_checkpoint_archive_decode_with_workspace, UmiDataMigrationCheckpointTransferEqual)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiDataMigrationCheckpointConditionalArchiveCases, UmiDataMigrationCheckpoint,
    umi_data_migration_checkpoint_archive_encode, umi_data_migration_checkpoint_archive_decode_if_unchanged,
    UmiDataMigrationCheckpointTransferEqual, UmiDataMigrationCheckpointTransferTails, UmiDataMigrationCheckpointTransferMalformed)



/* A rejected replacement must preserve the previously accepted migration checkpoint value. */
static int UmiDataMigrationCheckpointConstructionCases(void)
{
    UmiDataMigrationCheckpoint *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_data_migration_checkpoint_init(value, "cp1", "mig1", 2U, 11U, 22U) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->checkpoint_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_migration_checkpoint_init(value, oversized, "mig1", 2U, 11U, 22U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_migration_checkpoint_init(value, value->checkpoint_id, "mig1", 2U, 11U, 22U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->checkpoint_id, "cp1") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->migration_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_migration_checkpoint_init(value, "cp1", oversized, 2U, 11U, 22U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_migration_checkpoint_init(value, "cp1", value->migration_id, 2U, 11U, 22U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->migration_id, "mig1") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiDataMigrationCheckpointConstructionCases() != 0) return 1;

    UmiDataMigrationCheckpoint item;
    CHECK(umi_data_migration_checkpoint_init(&item,"cp1","mig1",2U,11U,22U) == UMI_STATUS_OK);
    if (UmiDataMigrationCheckpointTransferCases(&item) != 0) return 1;
    if (UmiDataMigrationCheckpointTransferCasesWithWorkspace(&item) != 0) return 1;
    if (UmiDataMigrationCheckpointConditionalArchiveCases(&item) != 0) return 1;

    CHECK(item.completed_steps==2U);
    return 0;
}
