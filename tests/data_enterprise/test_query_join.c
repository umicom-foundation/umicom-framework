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

int main(void) {
    UmiDataQueryJoin item;
    CHECK(umi_data_query_join_init(&item,"j1","orders","customers","orders.customer_id=customers.id",false) == UMI_STATUS_OK);
    if (UmiDataQueryJoinTransferCases(&item) != 0) return 1;

    CHECK(!item.outer_join);
    return 0;
}
