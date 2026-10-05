/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_savepoint_plan.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the savepoint plan enterprise data capability.
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
#include "umicom/data/enterprise/savepoint_plan.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/savepoint_plan.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataSavepointPlanTransferEqual(const UmiDataSavepointPlan *a, const UmiDataSavepointPlan *b)
{
    return strcmp(a->savepoint_id, b->savepoint_id) == 0 &&
        a->ordinal == b->ordinal &&
        a->release_on_success == b->release_on_success &&
        a->rollback_on_failure == b->rollback_on_failure;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataSavepointPlanTransferTails(UmiDataSavepointPlan *value)
{
    (void)value;
    {
        size_t used = strlen(value->savepoint_id) + 1U;
        memset(value->savepoint_id + used, 0xa5, sizeof(value->savepoint_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataSavepointPlanTransferMalformed(const UmiDataSavepointPlan *sample)
{
    (void)sample;
    {
        UmiDataSavepointPlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.savepoint_id, 'x', sizeof(invalid.savepoint_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_savepoint_plan_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_savepoint_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated savepoint_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataSavepointPlanTransferCases, UmiDataSavepointPlan,
    umi_data_savepoint_plan_archive_encode, umi_data_savepoint_plan_archive_decode,
    UmiDataSavepointPlanTransferEqual, UmiDataSavepointPlanTransferTails, UmiDataSavepointPlanTransferMalformed)

int main(void) {
    UmiDataSavepointPlan item;
    CHECK(umi_data_savepoint_plan_init(&item,"sp1",1U) == UMI_STATUS_OK);
    if (UmiDataSavepointPlanTransferCases(&item) != 0) return 1;

    CHECK(item.release_on_success && item.rollback_on_failure);
    return 0;
}
