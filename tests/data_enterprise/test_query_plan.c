/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_query_plan.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the query plan enterprise data capability.
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
#include "umicom/data/enterprise/query_plan.h"
#include "../value_archive/construction_cases.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/query_plan.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataQueryPlanTransferEqual(const UmiDataQueryPlan *a, const UmiDataQueryPlan *b)
{
    return strcmp(a->plan_id, b->plan_id) == 0 &&
        strcmp(a->root_table, b->root_table) == 0 &&
        a->predicate_count == b->predicate_count &&
        a->projection_count == b->projection_count &&
        a->join_count == b->join_count &&
        a->order_count == b->order_count &&
        a->row_limit == b->row_limit &&
        a->read_only == b->read_only;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataQueryPlanTransferTails(UmiDataQueryPlan *value)
{
    (void)value;
    {
        size_t used = strlen(value->plan_id) + 1U;
        memset(value->plan_id + used, 0xa5, sizeof(value->plan_id) - used);
    }
    {
        size_t used = strlen(value->root_table) + 1U;
        memset(value->root_table + used, 0xa5, sizeof(value->root_table) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataQueryPlanTransferMalformed(const UmiDataQueryPlan *sample)
{
    (void)sample;
    {
        UmiDataQueryPlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.plan_id, 'x', sizeof(invalid.plan_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_query_plan_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_query_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated plan_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataQueryPlan invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.root_table, 'x', sizeof(invalid.root_table));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_query_plan_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_query_plan_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated root_table was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataQueryPlanTransferCases, UmiDataQueryPlan,
    umi_data_query_plan_archive_encode, umi_data_query_plan_archive_decode,
    UmiDataQueryPlanTransferEqual, UmiDataQueryPlanTransferTails, UmiDataQueryPlanTransferMalformed)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiDataQueryPlanConditionalArchiveCases, UmiDataQueryPlan,
    umi_data_query_plan_archive_encode, umi_data_query_plan_archive_decode_if_unchanged,
    UmiDataQueryPlanTransferEqual, UmiDataQueryPlanTransferTails, UmiDataQueryPlanTransferMalformed)



/* A rejected replacement must preserve the previously accepted query plan value. */
static int UmiDataQueryPlanConstructionCases(void)
{
    UmiDataQueryPlan *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_data_query_plan_init(value, "q1", "orders") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->plan_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_plan_init(value, oversized, "orders") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_plan_init(value, value->plan_id, "orders") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->plan_id, "q1") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->root_table) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_plan_init(value, "q1", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_plan_init(value, "q1", value->root_table) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->root_table, "orders") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiDataQueryPlanConstructionCases() != 0) return 1;

    UmiDataQueryPlan p; CHECK(umi_data_query_plan_init(&p,"q1","orders")==UMI_STATUS_OK); CHECK(umi_data_query_plan_shape(&p,2U,4U,1U,1U,100U)==UMI_STATUS_OK); CHECK(umi_data_query_plan_validate(&p)==UMI_STATUS_OK);
    if (UmiDataQueryPlanTransferCases(&p) != 0) return 1;
    if (UmiDataQueryPlanConditionalArchiveCases(&p) != 0) return 1;

    return 0;
}
