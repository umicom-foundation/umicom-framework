/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_schema_index.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the schema index enterprise data capability.
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
#include "umicom/data/enterprise/schema_index.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/schema_index.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataSchemaIndexTransferEqual(const UmiDataSchemaIndex *a, const UmiDataSchemaIndex *b)
{
    return strcmp(a->index_id, b->index_id) == 0 &&
        strcmp(a->table_id, b->table_id) == 0 &&
        strcmp(a->key_expression, b->key_expression) == 0 &&
        a->unique == b->unique &&
        a->covering == b->covering;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataSchemaIndexTransferTails(UmiDataSchemaIndex *value)
{
    (void)value;
    {
        size_t used = strlen(value->index_id) + 1U;
        memset(value->index_id + used, 0xa5, sizeof(value->index_id) - used);
    }
    {
        size_t used = strlen(value->table_id) + 1U;
        memset(value->table_id + used, 0xa5, sizeof(value->table_id) - used);
    }
    {
        size_t used = strlen(value->key_expression) + 1U;
        memset(value->key_expression + used, 0xa5, sizeof(value->key_expression) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataSchemaIndexTransferMalformed(const UmiDataSchemaIndex *sample)
{
    (void)sample;
    {
        UmiDataSchemaIndex invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.index_id, 'x', sizeof(invalid.index_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_schema_index_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_schema_index_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated index_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataSchemaIndex invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.table_id, 'x', sizeof(invalid.table_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_schema_index_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_schema_index_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated table_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataSchemaIndex invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.key_expression, 'x', sizeof(invalid.key_expression));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_schema_index_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_schema_index_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated key_expression was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataSchemaIndexTransferCases, UmiDataSchemaIndex,
    umi_data_schema_index_archive_encode, umi_data_schema_index_archive_decode,
    UmiDataSchemaIndexTransferEqual, UmiDataSchemaIndexTransferTails, UmiDataSchemaIndexTransferMalformed)

int main(void) {
    UmiDataSchemaIndex item;
    CHECK(umi_data_schema_index_init(&item,"orders.pk","orders","id",true) == UMI_STATUS_OK);
    if (UmiDataSchemaIndexTransferCases(&item) != 0) return 1;

    CHECK(item.unique);
    return 0;
}
