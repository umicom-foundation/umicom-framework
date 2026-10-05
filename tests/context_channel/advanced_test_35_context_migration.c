/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/advanced_test_35_context_migration.c
 *
 * PURPOSE:
 *   Validate context migration sequence accounting, bounded fields and failure evidence.
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
#include <assert.h>
#include <string.h>
#include "umicom/context_channel/context_migration.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/context_channel/context_migration.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextMigrationTransferEqual(const UmiContextMigration *a, const UmiContextMigration *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->migration_id, b->migration_id) == 0 &&
        strcmp(a->schema_id, b->schema_id) == 0 &&
        strcmp(a->from_version, b->from_version) == 0 &&
        strcmp(a->to_version, b->to_version) == 0 &&
        a->first_sequence == b->first_sequence &&
        a->last_sequence == b->last_sequence &&
        a->item_count == b->item_count &&
        a->failure_count == b->failure_count &&
        a->status == b->status &&
        a->enabled == b->enabled &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextMigrationTransferTails(UmiContextMigration *value)
{
    (void)value;
    {
        size_t used = strlen(value->migration_id) + 1U;
        memset(value->migration_id + used, 0xa5, sizeof(value->migration_id) - used);
    }
    {
        size_t used = strlen(value->schema_id) + 1U;
        memset(value->schema_id + used, 0xa5, sizeof(value->schema_id) - used);
    }
    {
        size_t used = strlen(value->from_version) + 1U;
        memset(value->from_version + used, 0xa5, sizeof(value->from_version) - used);
    }
    {
        size_t used = strlen(value->to_version) + 1U;
        memset(value->to_version + used, 0xa5, sizeof(value->to_version) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextMigrationTransferMalformed(const UmiContextMigration *sample)
{
    (void)sample;
    {
        UmiContextMigration invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.migration_id, 'x', sizeof(invalid.migration_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_migration_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_migration_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated migration_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextMigration invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.schema_id, 'x', sizeof(invalid.schema_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_migration_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_migration_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated schema_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextMigration invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.from_version, 'x', sizeof(invalid.from_version));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_migration_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_migration_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated from_version was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextMigration invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.to_version, 'x', sizeof(invalid.to_version));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_migration_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_migration_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated to_version was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextMigrationTransferCases, UmiContextMigration,
    umi_context_migration_archive_encode, umi_context_migration_archive_decode,
    UmiContextMigrationTransferEqual, UmiContextMigrationTransferTails, UmiContextMigrationTransferMalformed)

int main(void)
{
    UmiContextMigration state;
    umi_context_migration_init(&state);
    assert(umi_context_migration_set_field(&state,0U,"alpha") == UMI_STATUS_OK);
    assert(strcmp(umi_context_migration_field(&state,0U),"alpha") == 0);
    assert(umi_context_migration_record_success(&state,10U) == UMI_STATUS_OK);
    assert(umi_context_migration_record_failure(&state,UMI_STATUS_TIMEOUT,11U) == UMI_STATUS_OK);
    assert(state.item_count == 2U);
    assert(state.failure_count == 1U);
    assert(umi_context_migration_covers_sequence(&state,10U));
    assert(umi_context_migration_covers_sequence(&state,11U));
    assert(umi_context_migration_validate(&state) == UMI_STATUS_OK);
    if (UmiContextMigrationTransferCases(&state) != 0) return 1;

    return 0;
}
