/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_schema_column.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the schema column enterprise data capability.
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
#include "umicom/data/enterprise/schema_column.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/schema_column.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataSchemaColumnTransferEqual(const UmiDataSchemaColumn *a, const UmiDataSchemaColumn *b)
{
    return strcmp(a->column_id, b->column_id) == 0 &&
        strcmp(a->name, b->name) == 0 &&
        a->kind == b->kind &&
        a->ordinal == b->ordinal &&
        a->nullable == b->nullable &&
        a->generated == b->generated;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataSchemaColumnTransferTails(UmiDataSchemaColumn *value)
{
    (void)value;
    {
        size_t used = strlen(value->column_id) + 1U;
        memset(value->column_id + used, 0xa5, sizeof(value->column_id) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataSchemaColumnTransferMalformed(const UmiDataSchemaColumn *sample)
{
    (void)sample;
    {
        UmiDataSchemaColumn invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.column_id, 'x', sizeof(invalid.column_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_schema_column_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_schema_column_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated column_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataSchemaColumn invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_schema_column_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_schema_column_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataSchemaColumnTransferCases, UmiDataSchemaColumn,
    umi_data_schema_column_archive_encode, umi_data_schema_column_archive_decode,
    UmiDataSchemaColumnTransferEqual, UmiDataSchemaColumnTransferTails, UmiDataSchemaColumnTransferMalformed)

int main(void) {
    UmiDataSchemaColumn item;
    CHECK(umi_data_schema_column_init(&item,"order.id","id",UMI_DATA_VALUE_INTEGER,0U,false) == UMI_STATUS_OK);
    if (UmiDataSchemaColumnTransferCases(&item) != 0) return 1;

    CHECK(item.nullable == false);
    return 0;
}
