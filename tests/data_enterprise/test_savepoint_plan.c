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
#include "../value_archive/construction_cases.h"
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

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiDataSavepointPlanConditionalArchiveCases, UmiDataSavepointPlan,
    umi_data_savepoint_plan_archive_encode, umi_data_savepoint_plan_archive_decode_if_unchanged,
    UmiDataSavepointPlanTransferEqual, UmiDataSavepointPlanTransferTails, UmiDataSavepointPlanTransferMalformed)



/* A rejected replacement must preserve the previously accepted savepoint plan value. */
static int UmiDataSavepointPlanConstructionCases(void)
{
    UmiDataSavepointPlan *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_data_savepoint_plan_init(value, "sp1", 1U) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->savepoint_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_savepoint_plan_init(value, oversized, 1U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_savepoint_plan_init(value, value->savepoint_id, 1U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->savepoint_id, "sp1") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiDataSavepointPlanConstructionCases() != 0) return 1;

    UmiDataSavepointPlan item;
    CHECK(umi_data_savepoint_plan_init(&item,"sp1",1U) == UMI_STATUS_OK);
    if (UmiDataSavepointPlanTransferCases(&item) != 0) return 1;
    if (UmiDataSavepointPlanConditionalArchiveCases(&item) != 0) return 1;

    CHECK(item.release_on_success && item.rollback_on_failure);
    return 0;
}
