/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_query_order.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the query order enterprise data capability.
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
#include "umicom/data/enterprise/query_order.h"
#include "../value_archive/construction_cases.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/query_order.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataQueryOrderTransferEqual(const UmiDataQueryOrder *a, const UmiDataQueryOrder *b)
{
    return strcmp(a->order_id, b->order_id) == 0 &&
        strcmp(a->field, b->field) == 0 &&
        a->descending == b->descending &&
        a->nulls_last == b->nulls_last;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataQueryOrderTransferTails(UmiDataQueryOrder *value)
{
    (void)value;
    {
        size_t used = strlen(value->order_id) + 1U;
        memset(value->order_id + used, 0xa5, sizeof(value->order_id) - used);
    }
    {
        size_t used = strlen(value->field) + 1U;
        memset(value->field + used, 0xa5, sizeof(value->field) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataQueryOrderTransferMalformed(const UmiDataQueryOrder *sample)
{
    (void)sample;
    {
        UmiDataQueryOrder invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.order_id, 'x', sizeof(invalid.order_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_query_order_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_query_order_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated order_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataQueryOrder invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.field, 'x', sizeof(invalid.field));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_query_order_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_query_order_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated field was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataQueryOrderTransferCases, UmiDataQueryOrder,
    umi_data_query_order_archive_encode, umi_data_query_order_archive_decode,
    UmiDataQueryOrderTransferEqual, UmiDataQueryOrderTransferTails, UmiDataQueryOrderTransferMalformed)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiDataQueryOrderConditionalArchiveCases, UmiDataQueryOrder,
    umi_data_query_order_archive_encode, umi_data_query_order_archive_decode_if_unchanged,
    UmiDataQueryOrderTransferEqual, UmiDataQueryOrderTransferTails, UmiDataQueryOrderTransferMalformed)



/* A rejected replacement must preserve the previously accepted query order value. */
static int UmiDataQueryOrderConstructionCases(void)
{
    UmiDataQueryOrder *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_data_query_order_init(value, "o1", "created_at", true) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->order_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_order_init(value, oversized, "created_at", true) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_order_init(value, value->order_id, "created_at", true) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->order_id, "o1") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->field) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_order_init(value, "o1", oversized, true) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_order_init(value, "o1", value->field, true) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->field, "created_at") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiDataQueryOrderConstructionCases() != 0) return 1;

    UmiDataQueryOrder item;
    CHECK(umi_data_query_order_init(&item,"o1","created_at",true) == UMI_STATUS_OK);
    if (UmiDataQueryOrderTransferCases(&item) != 0) return 1;
    if (UmiDataQueryOrderConditionalArchiveCases(&item) != 0) return 1;

    CHECK(item.descending);
    return 0;
}
