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
#include "../value_archive/construction_cases.h"
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

/* Exercise the caller-workspace API using this owner\'s real fixture and field comparator. */
#include "../value_archive/workspace_cases.h"
UMI_TEST_VALUE_WORKSPACE(UmiDataSchemaIdentifierTransferCasesWithWorkspace, UmiDataSchemaIdentifier,
    umi_data_schema_identifier_archive_encode, umi_data_schema_identifier_archive_decode_with_workspace, UmiDataSchemaIdentifierTransferEqual)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiDataSchemaIdentifierConditionalArchiveCases, UmiDataSchemaIdentifier,
    umi_data_schema_identifier_archive_encode, umi_data_schema_identifier_archive_decode_if_unchanged,
    UmiDataSchemaIdentifierTransferEqual, UmiDataSchemaIdentifierTransferTails, UmiDataSchemaIdentifierTransferMalformed)



/* A rejected replacement must preserve the previously accepted schema identifier value. */
static int UmiDataSchemaIdentifierConstructionCases(void)
{
    UmiDataSchemaIdentifier *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_data_schema_identifier_init(value, "main", "public", "orders") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->catalog) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_schema_identifier_init(value, oversized, "public", "orders") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_schema_identifier_init(value, value->catalog, "public", "orders") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->catalog, "main") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->schema) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_schema_identifier_init(value, "main", oversized, "orders") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_schema_identifier_init(value, "main", value->schema, "orders") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->schema, "public") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->name) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_schema_identifier_init(value, "main", "public", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_schema_identifier_init(value, "main", "public", value->name) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->name, "orders") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiDataSchemaIdentifierConstructionCases() != 0) return 1;

    UmiDataSchemaIdentifier item;
    CHECK(umi_data_schema_identifier_init(&item, "main", "public", "orders") == UMI_STATUS_OK);
    if (UmiDataSchemaIdentifierTransferCases(&item) != 0) return 1;
    if (UmiDataSchemaIdentifierTransferCasesWithWorkspace(&item) != 0) return 1;
    if (UmiDataSchemaIdentifierConditionalArchiveCases(&item) != 0) return 1;

    CHECK(strcmp(item.name,"orders")==0);
    return 0;
}
