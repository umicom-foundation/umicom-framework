/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_query_join.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the query join enterprise data capability.
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
#include "umicom/data/enterprise/query_join.h"
#include "../value_archive/construction_cases.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/query_join.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataQueryJoinTransferEqual(const UmiDataQueryJoin *a, const UmiDataQueryJoin *b)
{
    return strcmp(a->join_id, b->join_id) == 0 &&
        strcmp(a->left_table, b->left_table) == 0 &&
        strcmp(a->right_table, b->right_table) == 0 &&
        strcmp(a->condition, b->condition) == 0 &&
        a->outer_join == b->outer_join;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataQueryJoinTransferTails(UmiDataQueryJoin *value)
{
    (void)value;
    {
        size_t used = strlen(value->join_id) + 1U;
        memset(value->join_id + used, 0xa5, sizeof(value->join_id) - used);
    }
    {
        size_t used = strlen(value->left_table) + 1U;
        memset(value->left_table + used, 0xa5, sizeof(value->left_table) - used);
    }
    {
        size_t used = strlen(value->right_table) + 1U;
        memset(value->right_table + used, 0xa5, sizeof(value->right_table) - used);
    }
    {
        size_t used = strlen(value->condition) + 1U;
        memset(value->condition + used, 0xa5, sizeof(value->condition) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataQueryJoinTransferMalformed(const UmiDataQueryJoin *sample)
{
    (void)sample;
    {
        UmiDataQueryJoin invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.join_id, 'x', sizeof(invalid.join_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_query_join_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_query_join_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated join_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataQueryJoin invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.left_table, 'x', sizeof(invalid.left_table));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_query_join_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_query_join_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated left_table was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataQueryJoin invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.right_table, 'x', sizeof(invalid.right_table));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_query_join_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_query_join_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated right_table was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataQueryJoin invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.condition, 'x', sizeof(invalid.condition));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_query_join_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_query_join_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated condition was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataQueryJoinTransferCases, UmiDataQueryJoin,
    umi_data_query_join_archive_encode, umi_data_query_join_archive_decode,
    UmiDataQueryJoinTransferEqual, UmiDataQueryJoinTransferTails, UmiDataQueryJoinTransferMalformed)

/* Exercise the caller-workspace API using this owner\'s real fixture and field comparator. */
#include "../value_archive/workspace_cases.h"
UMI_TEST_VALUE_WORKSPACE(UmiDataQueryJoinTransferCasesWithWorkspace, UmiDataQueryJoin,
    umi_data_query_join_archive_encode, umi_data_query_join_archive_decode_with_workspace, UmiDataQueryJoinTransferEqual)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiDataQueryJoinConditionalArchiveCases, UmiDataQueryJoin,
    umi_data_query_join_archive_encode, umi_data_query_join_archive_decode_if_unchanged,
    UmiDataQueryJoinTransferEqual, UmiDataQueryJoinTransferTails, UmiDataQueryJoinTransferMalformed)



/* A rejected replacement must preserve the previously accepted query join value. */
static int UmiDataQueryJoinConstructionCases(void)
{
    UmiDataQueryJoin *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_data_query_join_init(value, "j1", "orders", "customers", "orders.customer_id=customers.id", false) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->join_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_join_init(value, oversized, "orders", "customers", "orders.customer_id=customers.id", false) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_join_init(value, value->join_id, "orders", "customers", "orders.customer_id=customers.id", false) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->join_id, "j1") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->left_table) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_join_init(value, "j1", oversized, "customers", "orders.customer_id=customers.id", false) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_join_init(value, "j1", value->left_table, "customers", "orders.customer_id=customers.id", false) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->left_table, "orders") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->right_table) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_join_init(value, "j1", "orders", oversized, "orders.customer_id=customers.id", false) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_join_init(value, "j1", "orders", value->right_table, "orders.customer_id=customers.id", false) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->right_table, "customers") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->condition) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_join_init(value, "j1", "orders", "customers", oversized, false) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_join_init(value, "j1", "orders", "customers", value->condition, false) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->condition, "orders.customer_id=customers.id") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiDataQueryJoinConstructionCases() != 0) return 1;

    UmiDataQueryJoin item;
    CHECK(umi_data_query_join_init(&item,"j1","orders","customers","orders.customer_id=customers.id",false) == UMI_STATUS_OK);
    if (UmiDataQueryJoinTransferCases(&item) != 0) return 1;
    if (UmiDataQueryJoinTransferCasesWithWorkspace(&item) != 0) return 1;
    if (UmiDataQueryJoinConditionalArchiveCases(&item) != 0) return 1;

    CHECK(!item.outer_join);
    return 0;
}
