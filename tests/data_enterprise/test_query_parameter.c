/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_query_parameter.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the query parameter enterprise data capability.
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
#include "umicom/data/enterprise/query_parameter.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/query_parameter.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataQueryParameterTransferEqual(const UmiDataQueryParameter *a, const UmiDataQueryParameter *b)
{
    return strcmp(a->parameter_id, b->parameter_id) == 0 &&
        strcmp(a->name, b->name) == 0 &&
        a->kind == b->kind &&
        strcmp(a->value, b->value) == 0 &&
        a->sensitive == b->sensitive;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataQueryParameterTransferTails(UmiDataQueryParameter *value)
{
    (void)value;
    {
        size_t used = strlen(value->parameter_id) + 1U;
        memset(value->parameter_id + used, 0xa5, sizeof(value->parameter_id) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->value) + 1U;
        memset(value->value + used, 0xa5, sizeof(value->value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataQueryParameterTransferMalformed(const UmiDataQueryParameter *sample)
{
    (void)sample;
    {
        UmiDataQueryParameter invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.parameter_id, 'x', sizeof(invalid.parameter_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_query_parameter_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_query_parameter_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated parameter_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataQueryParameter invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_query_parameter_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_query_parameter_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataQueryParameter invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value, 'x', sizeof(invalid.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_query_parameter_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_query_parameter_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataQueryParameterTransferCases, UmiDataQueryParameter,
    umi_data_query_parameter_archive_encode, umi_data_query_parameter_archive_decode,
    UmiDataQueryParameterTransferEqual, UmiDataQueryParameterTransferTails, UmiDataQueryParameterTransferMalformed)

int main(void) {
    UmiDataQueryParameter item;
    CHECK(umi_data_query_parameter_init(&item,"p1","status",UMI_DATA_VALUE_TEXT,"OPEN",false) == UMI_STATUS_OK);
    if (UmiDataQueryParameterTransferCases(&item) != 0) return 1;

    CHECK(strcmp(item.value,"OPEN")==0);
    return 0;
}
