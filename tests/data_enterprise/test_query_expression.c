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

int main(void) {
    UmiDataQueryExpression item;
    CHECK(umi_data_query_expression_init(&item,"e1","status","=","OPEN") == UMI_STATUS_OK);
    if (UmiDataQueryExpressionTransferCases(&item) != 0) return 1;

    CHECK(item.parameterized);
    return 0;
}
