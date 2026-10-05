/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_schema_identifier.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the schema identifier enterprise data capability.
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
#include "umicom/data/enterprise/schema_identifier.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/schema_identifier.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataSchemaIdentifierTransferEqual(const UmiDataSchemaIdentifier *a, const UmiDataSchemaIdentifier *b)
{
    return strcmp(a->catalog, b->catalog) == 0 &&
        strcmp(a->schema, b->schema) == 0 &&
        strcmp(a->name, b->name) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataSchemaIdentifierTransferTails(UmiDataSchemaIdentifier *value)
{
    (void)value;
    {
        size_t used = strlen(value->catalog) + 1U;
        memset(value->catalog + used, 0xa5, sizeof(value->catalog) - used);
    }
    {
        size_t used = strlen(value->schema) + 1U;
        memset(value->schema + used, 0xa5, sizeof(value->schema) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataSchemaIdentifierTransferMalformed(const UmiDataSchemaIdentifier *sample)
{
    (void)sample;
    {
        UmiDataSchemaIdentifier invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.catalog, 'x', sizeof(invalid.catalog));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_schema_identifier_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_schema_identifier_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated catalog was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataSchemaIdentifier invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.schema, 'x', sizeof(invalid.schema));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_schema_identifier_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_schema_identifier_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated schema was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataSchemaIdentifier invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_schema_identifier_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_schema_identifier_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataSchemaIdentifierTransferCases, UmiDataSchemaIdentifier,
    umi_data_schema_identifier_archive_encode, umi_data_schema_identifier_archive_decode,
    UmiDataSchemaIdentifierTransferEqual, UmiDataSchemaIdentifierTransferTails, UmiDataSchemaIdentifierTransferMalformed)

int main(void) {
    UmiDataSchemaIdentifier item;
    CHECK(umi_data_schema_identifier_init(&item, "main", "public", "orders") == UMI_STATUS_OK);
    if (UmiDataSchemaIdentifierTransferCases(&item) != 0) return 1;

    CHECK(strcmp(item.name,"orders")==0);
    return 0;
}
