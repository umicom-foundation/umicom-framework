/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_filter_expression.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the filter expression Integration Fabric capability.
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
#include "umicom/integration/fabric/filter_expression.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/integration/fabric/filter_expression.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFabricFilterExpressionTransferEqual(const UmiFabricFilterExpression *a, const UmiFabricFilterExpression *b)
{
    return strcmp(a->field, b->field) == 0 &&
        strcmp(a->operation, b->operation) == 0 &&
        strcmp(a->value, b->value) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFabricFilterExpressionTransferTails(UmiFabricFilterExpression *value)
{
    (void)value;
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
static int UmiFabricFilterExpressionTransferMalformed(const UmiFabricFilterExpression *sample)
{
    (void)sample;
    {
        UmiFabricFilterExpression invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.field, 'x', sizeof(invalid.field));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_filter_expression_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_filter_expression_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated field was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricFilterExpression invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.operation, 'x', sizeof(invalid.operation));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_filter_expression_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_filter_expression_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated operation was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricFilterExpression invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value, 'x', sizeof(invalid.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_filter_expression_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_filter_expression_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFabricFilterExpressionTransferCases, UmiFabricFilterExpression,
    umi_fabric_filter_expression_archive_encode, umi_fabric_filter_expression_archive_decode,
    UmiFabricFilterExpressionTransferEqual, UmiFabricFilterExpressionTransferTails, UmiFabricFilterExpressionTransferMalformed)

int main(void) {
    UmiFabricFilterExpression item;
    CHECK(umi_fabric_filter_expression_init(&item,"type","prefix","trade.")==UMI_STATUS_OK);
    if (UmiFabricFilterExpressionTransferCases(&item) != 0) return 1;

    CHECK(strcmp(item.operation,"prefix")==0);
    return 0;
}
