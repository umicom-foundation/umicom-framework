/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_migration_step.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the migration step enterprise data capability.
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
#include "umicom/data/enterprise/migration_step.h"
#include "../value_archive/construction_cases.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/migration_step.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataMigrationStepTransferEqual(const UmiDataMigrationStep *a, const UmiDataMigrationStep *b)
{
    return strcmp(a->step_id, b->step_id) == 0 &&
        strcmp(a->description, b->description) == 0 &&
        a->ordinal == b->ordinal &&
        a->reversible == b->reversible &&
        a->destructive == b->destructive;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataMigrationStepTransferTails(UmiDataMigrationStep *value)
{
    (void)value;
    {
        size_t used = strlen(value->step_id) + 1U;
        memset(value->step_id + used, 0xa5, sizeof(value->step_id) - used);
    }
    {
        size_t used = strlen(value->description) + 1U;
        memset(value->description + used, 0xa5, sizeof(value->description) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataMigrationStepTransferMalformed(const UmiDataMigrationStep *sample)
{
    (void)sample;
    {
        UmiDataMigrationStep invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.step_id, 'x', sizeof(invalid.step_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_migration_step_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_migration_step_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated step_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataMigrationStep invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.description, 'x', sizeof(invalid.description));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_migration_step_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_migration_step_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated description was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataMigrationStepTransferCases, UmiDataMigrationStep,
    umi_data_migration_step_archive_encode, umi_data_migration_step_archive_decode,
    UmiDataMigrationStepTransferEqual, UmiDataMigrationStepTransferTails, UmiDataMigrationStepTransferMalformed)

/* Exercise the caller-workspace API using this owner\'s real fixture and field comparator. */
#include "../value_archive/workspace_cases.h"
UMI_TEST_VALUE_WORKSPACE(UmiDataMigrationStepTransferCasesWithWorkspace, UmiDataMigrationStep,
    umi_data_migration_step_archive_encode, umi_data_migration_step_archive_decode_with_workspace, UmiDataMigrationStepTransferEqual)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiDataMigrationStepConditionalArchiveCases, UmiDataMigrationStep,
    umi_data_migration_step_archive_encode, umi_data_migration_step_archive_decode_if_unchanged,
    UmiDataMigrationStepTransferEqual, UmiDataMigrationStepTransferTails, UmiDataMigrationStepTransferMalformed)



/* A rejected replacement must preserve the previously accepted migration step value. */
static int UmiDataMigrationStepConstructionCases(void)
{
    UmiDataMigrationStep *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_data_migration_step_init(value, "m1", "add orders status", 1U, true, false) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->step_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_migration_step_init(value, oversized, "add orders status", 1U, true, false) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_migration_step_init(value, value->step_id, "add orders status", 1U, true, false) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->step_id, "m1") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->description) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_migration_step_init(value, "m1", oversized, 1U, true, false) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_migration_step_init(value, "m1", value->description, 1U, true, false) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->description, "add orders status") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiDataMigrationStepConstructionCases() != 0) return 1;

    UmiDataMigrationStep item;
    CHECK(umi_data_migration_step_init(&item,"m1","add orders status",1U,true,false) == UMI_STATUS_OK);
    if (UmiDataMigrationStepTransferCases(&item) != 0) return 1;
    if (UmiDataMigrationStepTransferCasesWithWorkspace(&item) != 0) return 1;
    if (UmiDataMigrationStepConditionalArchiveCases(&item) != 0) return 1;

    CHECK(item.reversible && !item.destructive);
    return 0;
}
