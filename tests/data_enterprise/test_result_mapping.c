/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_result_mapping.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the result mapping enterprise data capability.
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
#include "umicom/data/enterprise/result_mapping.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/result_mapping.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataResultMappingTransferEqual(const UmiDataResultMapping *a, const UmiDataResultMapping *b)
{
    return strcmp(a->mapping_id, b->mapping_id) == 0 &&
        strcmp(a->entity_id, b->entity_id) == 0 &&
        strcmp(a->field_name, b->field_name) == 0 &&
        a->column_ordinal == b->column_ordinal &&
        a->kind == b->kind;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataResultMappingTransferTails(UmiDataResultMapping *value)
{
    (void)value;
    {
        size_t used = strlen(value->mapping_id) + 1U;
        memset(value->mapping_id + used, 0xa5, sizeof(value->mapping_id) - used);
    }
    {
        size_t used = strlen(value->entity_id) + 1U;
        memset(value->entity_id + used, 0xa5, sizeof(value->entity_id) - used);
    }
    {
        size_t used = strlen(value->field_name) + 1U;
        memset(value->field_name + used, 0xa5, sizeof(value->field_name) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataResultMappingTransferMalformed(const UmiDataResultMapping *sample)
{
    (void)sample;
    {
        UmiDataResultMapping invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.mapping_id, 'x', sizeof(invalid.mapping_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_result_mapping_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_result_mapping_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated mapping_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataResultMapping invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.entity_id, 'x', sizeof(invalid.entity_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_result_mapping_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_result_mapping_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated entity_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataResultMapping invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.field_name, 'x', sizeof(invalid.field_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_result_mapping_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_result_mapping_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated field_name was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataResultMappingTransferCases, UmiDataResultMapping,
    umi_data_result_mapping_archive_encode, umi_data_result_mapping_archive_decode,
    UmiDataResultMappingTransferEqual, UmiDataResultMappingTransferTails, UmiDataResultMappingTransferMalformed)

int main(void) {
    UmiDataResultMapping item;
    CHECK(umi_data_result_mapping_init(&item,"m1","Order","id",0U,UMI_DATA_VALUE_INTEGER) == UMI_STATUS_OK);
    if (UmiDataResultMappingTransferCases(&item) != 0) return 1;

    CHECK(item.column_ordinal==0U);
    return 0;
}
