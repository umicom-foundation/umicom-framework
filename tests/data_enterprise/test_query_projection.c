/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_query_projection.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the query projection enterprise data capability.
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
#include "umicom/data/enterprise/query_projection.h"
#include "../value_archive/construction_cases.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/query_projection.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataQueryProjectionTransferEqual(const UmiDataQueryProjection *a, const UmiDataQueryProjection *b)
{
    return strcmp(a->projection_id, b->projection_id) == 0 &&
        strcmp(a->field, b->field) == 0 &&
        strcmp(a->alias, b->alias) == 0 &&
        a->hidden == b->hidden;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataQueryProjectionTransferTails(UmiDataQueryProjection *value)
{
    (void)value;
    {
        size_t used = strlen(value->projection_id) + 1U;
        memset(value->projection_id + used, 0xa5, sizeof(value->projection_id) - used);
    }
    {
        size_t used = strlen(value->field) + 1U;
        memset(value->field + used, 0xa5, sizeof(value->field) - used);
    }
    {
        size_t used = strlen(value->alias) + 1U;
        memset(value->alias + used, 0xa5, sizeof(value->alias) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataQueryProjectionTransferMalformed(const UmiDataQueryProjection *sample)
{
    (void)sample;
    {
        UmiDataQueryProjection invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.projection_id, 'x', sizeof(invalid.projection_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_query_projection_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_query_projection_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated projection_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataQueryProjection invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.field, 'x', sizeof(invalid.field));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_query_projection_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_query_projection_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated field was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataQueryProjection invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.alias, 'x', sizeof(invalid.alias));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_query_projection_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_query_projection_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated alias was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataQueryProjectionTransferCases, UmiDataQueryProjection,
    umi_data_query_projection_archive_encode, umi_data_query_projection_archive_decode,
    UmiDataQueryProjectionTransferEqual, UmiDataQueryProjectionTransferTails, UmiDataQueryProjectionTransferMalformed)

/* Exercise the caller-workspace API using this owner\'s real fixture and field comparator. */
#include "../value_archive/workspace_cases.h"
UMI_TEST_VALUE_WORKSPACE(UmiDataQueryProjectionTransferCasesWithWorkspace, UmiDataQueryProjection,
    umi_data_query_projection_archive_encode, umi_data_query_projection_archive_decode_with_workspace, UmiDataQueryProjectionTransferEqual)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiDataQueryProjectionConditionalArchiveCases, UmiDataQueryProjection,
    umi_data_query_projection_archive_encode, umi_data_query_projection_archive_decode_if_unchanged,
    UmiDataQueryProjectionTransferEqual, UmiDataQueryProjectionTransferTails, UmiDataQueryProjectionTransferMalformed)



/* A rejected replacement must preserve the previously accepted query projection value. */
static int UmiDataQueryProjectionConstructionCases(void)
{
    UmiDataQueryProjection *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_data_query_projection_init(value, "p1", "orders.id", "id") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->projection_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_projection_init(value, oversized, "orders.id", "id") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_projection_init(value, value->projection_id, "orders.id", "id") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->projection_id, "p1") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->field) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_projection_init(value, "p1", oversized, "id") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_projection_init(value, "p1", value->field, "id") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->field, "orders.id") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->alias) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_projection_init(value, "p1", "orders.id", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_query_projection_init(value, "p1", "orders.id", value->alias) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->alias, "id") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiDataQueryProjectionConstructionCases() != 0) return 1;

    UmiDataQueryProjection item;
    CHECK(umi_data_query_projection_init(&item,"p1","orders.id","id") == UMI_STATUS_OK);
    if (UmiDataQueryProjectionTransferCases(&item) != 0) return 1;
    if (UmiDataQueryProjectionTransferCasesWithWorkspace(&item) != 0) return 1;
    if (UmiDataQueryProjectionConditionalArchiveCases(&item) != 0) return 1;

    CHECK(strcmp(item.alias,"id")==0);
    return 0;
}
