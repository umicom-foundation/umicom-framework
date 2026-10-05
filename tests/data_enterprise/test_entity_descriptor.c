/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_entity_descriptor.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the entity descriptor enterprise data capability.
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
#include "umicom/data/enterprise/entity_descriptor.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/data/enterprise/entity_descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDataEntityDescriptorTransferEqual(const UmiDataEntityDescriptor *a, const UmiDataEntityDescriptor *b)
{
    return strcmp(a->entity_id, b->entity_id) == 0 &&
        strcmp(a->table_id, b->table_id) == 0 &&
        strcmp(a->identity_field, b->identity_field) == 0 &&
        a->field_count == b->field_count &&
        a->immutable == b->immutable;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDataEntityDescriptorTransferTails(UmiDataEntityDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->entity_id) + 1U;
        memset(value->entity_id + used, 0xa5, sizeof(value->entity_id) - used);
    }
    {
        size_t used = strlen(value->table_id) + 1U;
        memset(value->table_id + used, 0xa5, sizeof(value->table_id) - used);
    }
    {
        size_t used = strlen(value->identity_field) + 1U;
        memset(value->identity_field + used, 0xa5, sizeof(value->identity_field) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDataEntityDescriptorTransferMalformed(const UmiDataEntityDescriptor *sample)
{
    (void)sample;
    {
        UmiDataEntityDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.entity_id, 'x', sizeof(invalid.entity_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_entity_descriptor_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_entity_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated entity_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataEntityDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.table_id, 'x', sizeof(invalid.table_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_entity_descriptor_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_entity_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated table_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDataEntityDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.identity_field, 'x', sizeof(invalid.identity_field));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_data_entity_descriptor_validate(&invalid) != UMI_STATUS_OK) ||
            umi_data_entity_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated identity_field was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDataEntityDescriptorTransferCases, UmiDataEntityDescriptor,
    umi_data_entity_descriptor_archive_encode, umi_data_entity_descriptor_archive_decode,
    UmiDataEntityDescriptorTransferEqual, UmiDataEntityDescriptorTransferTails, UmiDataEntityDescriptorTransferMalformed)

int main(void) {
    UmiDataEntityDescriptor item;
    CHECK(umi_data_entity_descriptor_init(&item,"Order","orders","id") == UMI_STATUS_OK);
    if (UmiDataEntityDescriptorTransferCases(&item) != 0) return 1;

    CHECK(strcmp(item.table_id,"orders")==0);
    return 0;
}
