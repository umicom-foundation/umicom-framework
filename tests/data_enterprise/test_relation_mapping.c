/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_relation_mapping.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the relation mapping enterprise data capability.
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
#include "umicom/data/enterprise/relation_mapping.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/relation_mapping.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataRelationMappingTransferEqual(const UmiDataRelationMapping *a, const UmiDataRelationMapping *b)
{
    return strcmp(a->relation_id, b->relation_id) == 0 &&
        strcmp(a->source_entity, b->source_entity) == 0 &&
        strcmp(a->target_entity, b->target_entity) == 0 &&
        strcmp(a->source_field, b->source_field) == 0 &&
        a->collection == b->collection &&
        a->required == b->required;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataRelationMappingTransferTails(UmiDataRelationMapping *value)
{
    (void)value;
    {
        size_t used = strlen(value->relation_id) + 1U;
        memset(value->relation_id + used, 0xa5, sizeof(value->relation_id) - used);
    }
    {
        size_t used = strlen(value->source_entity) + 1U;
        memset(value->source_entity + used, 0xa5, sizeof(value->source_entity) - used);
    }
    {
        size_t used = strlen(value->target_entity) + 1U;
        memset(value->target_entity + used, 0xa5, sizeof(value->target_entity) - used);
    }
    {
        size_t used = strlen(value->source_field) + 1U;
        memset(value->source_field + used, 0xa5, sizeof(value->source_field) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataRelationMappingTransferMalformed(const UmiDataRelationMapping *sample)
{
    (void)sample;
    {
        UmiDataRelationMapping invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.relation_id, 'x', sizeof(invalid.relation_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_relation_mapping_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_relation_mapping_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated relation_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataRelationMapping invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_entity, 'x', sizeof(invalid.source_entity));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_relation_mapping_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_relation_mapping_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_entity was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataRelationMapping invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_entity, 'x', sizeof(invalid.target_entity));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_relation_mapping_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_relation_mapping_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_entity was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataRelationMapping invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_field, 'x', sizeof(invalid.source_field));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_relation_mapping_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_relation_mapping_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_field was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataRelationMappingTransferCases, UmiDataRelationMapping,
    umi_data_relation_mapping_archive_encode, umi_data_relation_mapping_archive_decode,
    UmiDataRelationMappingTransferEqual, UmiDataRelationMappingTransferTails, UmiDataRelationMappingTransferMalformed)

int main(void) {
    UmiDataRelationMapping item;
    CHECK(umi_data_relation_mapping_init(&item,"Order.customer","Order","Customer","customer_id",false) == UMI_STATUS_OK);
    if (UmiDataRelationMappingTransferCases(&item) != 0) return 1;

    CHECK(!item.collection);
    return 0;
}
