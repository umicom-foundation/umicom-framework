/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_backup_plan.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the backup plan enterprise data capability.
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
#include "umicom/data/enterprise/backup_plan.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/backup_plan.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataBackupPlanTransferEqual(const UmiDataBackupPlan *a, const UmiDataBackupPlan *b)
{
    return strcmp(a->backup_id, b->backup_id) == 0 &&
        strcmp(a->destination, b->destination) == 0 &&
        a->schema_fingerprint == b->schema_fingerprint &&
        a->incremental == b->incremental &&
        a->include_blobs == b->include_blobs &&
        a->encrypted == b->encrypted;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataBackupPlanTransferTails(UmiDataBackupPlan *value)
{
    (void)value;
    {
        size_t used = strlen(value->backup_id) + 1U;
        memset(value->backup_id + used, 0xa5, sizeof(value->backup_id) - used);
    }
    {
        size_t used = strlen(value->destination) + 1U;
        memset(value->destination + used, 0xa5, sizeof(value->destination) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataBackupPlanTransferMalformed(const UmiDataBackupPlan *sample)
{
    (void)sample;
    {
        UmiDataBackupPlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.backup_id, 'x', sizeof(invalid.backup_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_backup_plan_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_backup_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated backup_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataBackupPlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.destination, 'x', sizeof(invalid.destination));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_backup_plan_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_backup_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated destination was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataBackupPlanTransferCases, UmiDataBackupPlan,
    umi_data_backup_plan_archive_encode, umi_data_backup_plan_archive_decode,
    UmiDataBackupPlanTransferEqual, UmiDataBackupPlanTransferTails, UmiDataBackupPlanTransferMalformed)

int main(void) {
    UmiDataBackupPlan item;
    CHECK(umi_data_backup_plan_init(&item,"b1","backup://daily",10U,false) == UMI_STATUS_OK);
    if (UmiDataBackupPlanTransferCases(&item) != 0) return 1;

    CHECK(item.encrypted);
    return 0;
}
