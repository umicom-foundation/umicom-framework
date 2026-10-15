/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_data_operation.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the data operation enterprise data capability.
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
#include "umicom/data/enterprise/data_operation.h"
#include "../value_archive/construction_cases.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/data_operation.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataOperationTransferEqual(const UmiDataOperation *a, const UmiDataOperation *b)
{
    return strcmp(a->operation_id, b->operation_id) == 0 &&
        strcmp(a->session_id, b->session_id) == 0 &&
        strcmp(a->operation_kind, b->operation_kind) == 0 &&
        a->submitted_at == b->submitted_at &&
        a->priority == b->priority &&
        a->cancellable == b->cancellable;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataOperationTransferTails(UmiDataOperation *value)
{
    (void)value;
    {
        size_t used = strlen(value->operation_id) + 1U;
        memset(value->operation_id + used, 0xa5, sizeof(value->operation_id) - used);
    }
    {
        size_t used = strlen(value->session_id) + 1U;
        memset(value->session_id + used, 0xa5, sizeof(value->session_id) - used);
    }
    {
        size_t used = strlen(value->operation_kind) + 1U;
        memset(value->operation_kind + used, 0xa5, sizeof(value->operation_kind) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataOperationTransferMalformed(const UmiDataOperation *sample)
{
    (void)sample;
    {
        UmiDataOperation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.operation_id, 'x', sizeof(invalid.operation_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_data_operation_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_data_operation_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated operation_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataOperation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.session_id, 'x', sizeof(invalid.session_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_data_operation_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_data_operation_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated session_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataOperation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.operation_kind, 'x', sizeof(invalid.operation_kind));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_data_operation_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_data_operation_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated operation_kind was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataOperationTransferCases, UmiDataOperation,
    umi_data_data_operation_archive_encode, umi_data_data_operation_archive_decode,
    UmiDataOperationTransferEqual, UmiDataOperationTransferTails, UmiDataOperationTransferMalformed)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiDataOperationConditionalArchiveCases, UmiDataOperation,
    umi_data_data_operation_archive_encode, umi_data_data_operation_archive_decode_if_unchanged,
    UmiDataOperationTransferEqual, UmiDataOperationTransferTails, UmiDataOperationTransferMalformed)



/* A rejected replacement must preserve the previously accepted data operation value. */
static int UmiDataOperationConstructionCases(void)
{
    UmiDataOperation *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_data_data_operation_init(value, "op1", "s1", "query", 10U, 5U) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->operation_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_data_operation_init(value, oversized, "s1", "query", 10U, 5U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_data_operation_init(value, value->operation_id, "s1", "query", 10U, 5U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->operation_id, "op1") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->session_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_data_operation_init(value, "op1", oversized, "query", 10U, 5U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_data_operation_init(value, "op1", value->session_id, "query", 10U, 5U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->session_id, "s1") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->operation_kind) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_data_operation_init(value, "op1", "s1", oversized, 10U, 5U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_data_operation_init(value, "op1", "s1", value->operation_kind, 10U, 5U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->operation_kind, "query") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiDataOperationConstructionCases() != 0) return 1;

    UmiDataOperation item;
    CHECK(umi_data_data_operation_init(&item,"op1","s1","query",10U,5U) == UMI_STATUS_OK);
    if (UmiDataOperationTransferCases(&item) != 0) return 1;
    if (UmiDataOperationConditionalArchiveCases(&item) != 0) return 1;

    CHECK(item.cancellable);
    return 0;
}
