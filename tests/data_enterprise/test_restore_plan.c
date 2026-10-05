/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_restore_plan.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the restore plan enterprise data capability.
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
#include "umicom/data/enterprise/restore_plan.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/restore_plan.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataRestorePlanTransferEqual(const UmiDataRestorePlan *a, const UmiDataRestorePlan *b)
{
    return strcmp(a->restore_id, b->restore_id) == 0 &&
        strcmp(a->backup_id, b->backup_id) == 0 &&
        a->expected_schema_fingerprint == b->expected_schema_fingerprint &&
        a->verify_only == b->verify_only &&
        a->preserve_existing == b->preserve_existing &&
        a->approved == b->approved;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataRestorePlanTransferTails(UmiDataRestorePlan *value)
{
    (void)value;
    {
        size_t used = strlen(value->restore_id) + 1U;
        memset(value->restore_id + used, 0xa5, sizeof(value->restore_id) - used);
    }
    {
        size_t used = strlen(value->backup_id) + 1U;
        memset(value->backup_id + used, 0xa5, sizeof(value->backup_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataRestorePlanTransferMalformed(const UmiDataRestorePlan *sample)
{
    (void)sample;
    {
        UmiDataRestorePlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.restore_id, 'x', sizeof(invalid.restore_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_restore_plan_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_restore_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated restore_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataRestorePlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.backup_id, 'x', sizeof(invalid.backup_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_restore_plan_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_restore_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated backup_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataRestorePlanTransferCases, UmiDataRestorePlan,
    umi_data_restore_plan_archive_encode, umi_data_restore_plan_archive_decode,
    UmiDataRestorePlanTransferEqual, UmiDataRestorePlanTransferTails, UmiDataRestorePlanTransferMalformed)

int main(void) {
    UmiDataRestorePlan item;
    CHECK(umi_data_restore_plan_init(&item,"r1","b1",11U,true) == UMI_STATUS_OK);
    if (UmiDataRestorePlanTransferCases(&item) != 0) return 1;

    CHECK(item.verify_only && item.preserve_existing);
    return 0;
}
