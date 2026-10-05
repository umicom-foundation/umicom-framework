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

int main(void) {
    UmiDataMigrationStep item;
    CHECK(umi_data_migration_step_init(&item,"m1","add orders status",1U,true,false) == UMI_STATUS_OK);
    if (UmiDataMigrationStepTransferCases(&item) != 0) return 1;

    CHECK(item.reversible && !item.destructive);
    return 0;
}
