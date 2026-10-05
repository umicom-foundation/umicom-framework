/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_schema_foreign_key.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the schema foreign key enterprise data capability.
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
#include "umicom/data/enterprise/schema_foreign_key.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/schema_foreign_key.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataSchemaForeignKeyTransferEqual(const UmiDataSchemaForeignKey *a, const UmiDataSchemaForeignKey *b)
{
    return strcmp(a->constraint_id, b->constraint_id) == 0 &&
        strcmp(a->source_table, b->source_table) == 0 &&
        strcmp(a->target_table, b->target_table) == 0 &&
        strcmp(a->source_column, b->source_column) == 0 &&
        strcmp(a->target_column, b->target_column) == 0 &&
        a->cascade_delete == b->cascade_delete;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataSchemaForeignKeyTransferTails(UmiDataSchemaForeignKey *value)
{
    (void)value;
    {
        size_t used = strlen(value->constraint_id) + 1U;
        memset(value->constraint_id + used, 0xa5, sizeof(value->constraint_id) - used);
    }
    {
        size_t used = strlen(value->source_table) + 1U;
        memset(value->source_table + used, 0xa5, sizeof(value->source_table) - used);
    }
    {
        size_t used = strlen(value->target_table) + 1U;
        memset(value->target_table + used, 0xa5, sizeof(value->target_table) - used);
    }
    {
        size_t used = strlen(value->source_column) + 1U;
        memset(value->source_column + used, 0xa5, sizeof(value->source_column) - used);
    }
    {
        size_t used = strlen(value->target_column) + 1U;
        memset(value->target_column + used, 0xa5, sizeof(value->target_column) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataSchemaForeignKeyTransferMalformed(const UmiDataSchemaForeignKey *sample)
{
    (void)sample;
    {
        UmiDataSchemaForeignKey invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.constraint_id, 'x', sizeof(invalid.constraint_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_schema_foreign_key_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_schema_foreign_key_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated constraint_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataSchemaForeignKey invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_table, 'x', sizeof(invalid.source_table));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_schema_foreign_key_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_schema_foreign_key_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_table was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataSchemaForeignKey invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_table, 'x', sizeof(invalid.target_table));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_schema_foreign_key_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_schema_foreign_key_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_table was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataSchemaForeignKey invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_column, 'x', sizeof(invalid.source_column));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_schema_foreign_key_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_schema_foreign_key_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_column was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataSchemaForeignKey invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_column, 'x', sizeof(invalid.target_column));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_schema_foreign_key_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_schema_foreign_key_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_column was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataSchemaForeignKeyTransferCases, UmiDataSchemaForeignKey,
    umi_data_schema_foreign_key_archive_encode, umi_data_schema_foreign_key_archive_decode,
    UmiDataSchemaForeignKeyTransferEqual, UmiDataSchemaForeignKeyTransferTails, UmiDataSchemaForeignKeyTransferMalformed)

int main(void) {
    UmiDataSchemaForeignKey item;
    CHECK(umi_data_schema_foreign_key_init(&item,"fk.order.customer","orders","customer_id","customers","id") == UMI_STATUS_OK);
    if (UmiDataSchemaForeignKeyTransferCases(&item) != 0) return 1;

    CHECK(strcmp(item.target_table,"customers")==0);
    return 0;
}
