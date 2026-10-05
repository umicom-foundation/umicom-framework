/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_field_mapping.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the field mapping enterprise data capability.
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
#include "umicom/data/enterprise/field_mapping.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/field_mapping.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataFieldMappingTransferEqual(const UmiDataFieldMapping *a, const UmiDataFieldMapping *b)
{
    return strcmp(a->mapping_id, b->mapping_id) == 0 &&
        strcmp(a->entity_id, b->entity_id) == 0 &&
        strcmp(a->field_name, b->field_name) == 0 &&
        strcmp(a->column_name, b->column_name) == 0 &&
        a->kind == b->kind &&
        a->nullable == b->nullable;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataFieldMappingTransferTails(UmiDataFieldMapping *value)
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
    {
        size_t used = strlen(value->column_name) + 1U;
        memset(value->column_name + used, 0xa5, sizeof(value->column_name) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataFieldMappingTransferMalformed(const UmiDataFieldMapping *sample)
{
    (void)sample;
    {
        UmiDataFieldMapping invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.mapping_id, 'x', sizeof(invalid.mapping_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_field_mapping_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_field_mapping_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated mapping_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataFieldMapping invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.entity_id, 'x', sizeof(invalid.entity_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_field_mapping_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_field_mapping_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated entity_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataFieldMapping invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.field_name, 'x', sizeof(invalid.field_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_field_mapping_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_field_mapping_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated field_name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataFieldMapping invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.column_name, 'x', sizeof(invalid.column_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_field_mapping_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_field_mapping_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated column_name was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataFieldMappingTransferCases, UmiDataFieldMapping,
    umi_data_field_mapping_archive_encode, umi_data_field_mapping_archive_decode,
    UmiDataFieldMappingTransferEqual, UmiDataFieldMappingTransferTails, UmiDataFieldMappingTransferMalformed)

int main(void) {
    UmiDataFieldMapping item;
    CHECK(umi_data_field_mapping_init(&item,"Order.id","Order","id","id",UMI_DATA_VALUE_INTEGER) == UMI_STATUS_OK);
    if (UmiDataFieldMappingTransferCases(&item) != 0) return 1;

    CHECK(item.kind==UMI_DATA_VALUE_INTEGER);
    return 0;
}
