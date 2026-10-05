/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_schema.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/schema.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/schema.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextSchemaTransferEqual(const UmiContextSchema *a, const UmiContextSchema *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->schema_id, b->schema_id) == 0 &&
        strcmp(a->display_name, b->display_name) == 0 &&
        strcmp(a->description, b->description) == 0 &&
        a->kind == b->kind &&
        a->schema_version == b->schema_version &&
        a->minimum_compatible_version == b->minimum_compatible_version &&
        a->sensitive == b->sensitive &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextSchemaTransferTails(UmiContextSchema *value)
{
    (void)value;
    {
        size_t used = strlen(value->schema_id) + 1U;
        memset(value->schema_id + used, 0xa5, sizeof(value->schema_id) - used);
    }
    {
        size_t used = strlen(value->display_name) + 1U;
        memset(value->display_name + used, 0xa5, sizeof(value->display_name) - used);
    }
    {
        size_t used = strlen(value->description) + 1U;
        memset(value->description + used, 0xa5, sizeof(value->description) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextSchemaTransferMalformed(const UmiContextSchema *sample)
{
    (void)sample;
    {
        UmiContextSchema invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.schema_id, 'x', sizeof(invalid.schema_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_schema_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_schema_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated schema_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextSchema invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.display_name, 'x', sizeof(invalid.display_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_schema_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_schema_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated display_name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextSchema invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.description, 'x', sizeof(invalid.description));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_schema_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_schema_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated description was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextSchemaTransferCases, UmiContextSchema,
    umi_context_schema_archive_encode, umi_context_schema_archive_decode,
    UmiContextSchemaTransferEqual, UmiContextSchemaTransferTails, UmiContextSchemaTransferMalformed)

int main(void)
{
    UmiContextSchema value;
    umi_context_schema_init(&value);
    value.schema_id[0] = 's';
    value.display_name[0] = 's';
    value.description[0] = 's';
    value.schema_version = (uint32_t)17U;
    value.minimum_compatible_version = (uint32_t)17U;
    value.sensitive = true;
    value.revision = (uint64_t)17U;
    if (umi_context_schema_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiContextSchemaTransferCases(&value) != 0) return 1;

    return 0;
}
