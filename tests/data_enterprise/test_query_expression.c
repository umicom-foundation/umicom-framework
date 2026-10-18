/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_query_expression.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the query expression enterprise data capability.
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
#include "umicom/data/enterprise/query_expression.h"
#include "../value_archive/construction_cases.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/query_expression.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataQueryExpressionTransferEqual(const UmiDataQueryExpression *a, const UmiDataQueryExpression *b)
{
    return strcmp(a->expression_id, b->expression_id) == 0 &&
        strcmp(a->field, b->field) == 0 &&
        strcmp(a->operation, b->operation) == 0 &&
        strcmp(a->value, b->value) == 0 &&
        a->parameterized == b->parameterized;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataQueryExpressionTransferTails(UmiDataQueryExpression *value)
{
    (void)value;
    {
        size_t used = strlen(value->expression_id) + 1U;
        memset(value->expression_id + used, 0xa5, sizeof(value->expression_id) - used);
    }
    {
        size_t used = strlen(value->field) + 1U;
        memset(value->field + used, 0xa5, sizeof(value->field) - used);
    }
    {
        size_t used = strlen(value->operation) + 1U;
        memset(value->operation + used, 0xa5, sizeof(value->operation) - used);
    }
    {
        size_t used = strlen(value->value) + 1U;
        memset(value->value + used, 0xa5, sizeof(value->value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataQueryExpressionTransferMalformed(const UmiDataQueryExpression *sample)
{
    (void)sample;
    {
        UmiDataQueryExpression invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.expression_id, 'x', sizeof(invalid.expression_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_query_expression_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_query_expression_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated expression_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataQueryExpression invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.field, 'x', sizeof(invalid.field));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_query_expression_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_query_expression_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated field was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataQueryExpression invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.operation, 'x', sizeof(invalid.operation));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_query_expression_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_query_expression_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated operation was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataQueryExpression invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value, 'x', sizeof(invalid.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_query_expression_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_query_expression_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataQueryExpressionTransferCases, UmiDataQueryExpression,
    umi_data_query_expression_archive_encode, umi_data_query_expression_archive_decode,
    UmiDataQueryExpressionTransferEqual, UmiDataQueryExpressionTransferTails, UmiDataQueryExpressionTransferMalformed)

/* Exercise the caller-workspace API using this owner\'s real fixture and field comparator. */
#include "../value_archive/workspace_cases.h"
UMI_TEST_VALUE_WORKSPACE(UmiDataQueryExpressionTransferCasesWithWorkspace, UmiDataQueryExpression,
    umi_data_query_expression_archive_encode, umi_data_query_expression_archive_decode_with_workspace, UmiDataQueryExpressionTransferEqual)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiDataQueryExpressionConditionalArchiveCases, UmiDataQueryExpression,
    umi_data_query_expression_archive_encode, umi_data_query_expression_archive_decode_if_unchanged,
    UmiDataQueryExpressionTransferEqual, UmiDataQueryExpressionTransferTails, UmiDataQueryExpressionTransferMalformed)



/* A rejected replacement must preserve the previously accepted query expression value. */
static int UmiDataQueryExpressionConstructionCases(void)
{
    UmiDataQueryExpression *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_data_query_expression_init(value, "e1", "status", "=", "OPEN") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->expression_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_expression_init(value, oversized, "status", "=", "OPEN") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_expression_init(value, value->expression_id, "status", "=", "OPEN") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->expression_id, "e1") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->field) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_expression_init(value, "e1", oversized, "=", "OPEN") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_expression_init(value, "e1", value->field, "=", "OPEN") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->field, "status") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->operation) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_expression_init(value, "e1", "status", oversized, "OPEN") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_expression_init(value, "e1", "status", value->operation, "OPEN") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->operation, "=") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->value) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_expression_init(value, "e1", "status", "=", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_expression_init(value, "e1", "status", "=", value->value) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->value, "OPEN") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiDataQueryExpressionConstructionCases() != 0) return 1;

    UmiDataQueryExpression item;
    CHECK(umi_data_query_expression_init(&item,"e1","status","=","OPEN") == UMI_STATUS_OK);
    if (UmiDataQueryExpressionTransferCases(&item) != 0) return 1;
    if (UmiDataQueryExpressionTransferCasesWithWorkspace(&item) != 0) return 1;
    if (UmiDataQueryExpressionConditionalArchiveCases(&item) != 0) return 1;

    CHECK(item.parameterized);
    return 0;
}
