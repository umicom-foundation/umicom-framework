/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_transformer_rule.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/transformer_rule.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/transformer_rule.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextTransformerRuleTransferEqual(const UmiContextTransformerRule *a, const UmiContextTransformerRule *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->transformer_id, b->transformer_id) == 0 &&
        strcmp(a->source_schema_id, b->source_schema_id) == 0 &&
        strcmp(a->target_schema_id, b->target_schema_id) == 0 &&
        strcmp(a->source_field, b->source_field) == 0 &&
        strcmp(a->target_field, b->target_field) == 0 &&
        a->enabled == b->enabled &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextTransformerRuleTransferTails(UmiContextTransformerRule *value)
{
    (void)value;
    {
        size_t used = strlen(value->transformer_id) + 1U;
        memset(value->transformer_id + used, 0xa5, sizeof(value->transformer_id) - used);
    }
    {
        size_t used = strlen(value->source_schema_id) + 1U;
        memset(value->source_schema_id + used, 0xa5, sizeof(value->source_schema_id) - used);
    }
    {
        size_t used = strlen(value->target_schema_id) + 1U;
        memset(value->target_schema_id + used, 0xa5, sizeof(value->target_schema_id) - used);
    }
    {
        size_t used = strlen(value->source_field) + 1U;
        memset(value->source_field + used, 0xa5, sizeof(value->source_field) - used);
    }
    {
        size_t used = strlen(value->target_field) + 1U;
        memset(value->target_field + used, 0xa5, sizeof(value->target_field) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextTransformerRuleTransferMalformed(const UmiContextTransformerRule *sample)
{
    (void)sample;
    {
        UmiContextTransformerRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.transformer_id, 'x', sizeof(invalid.transformer_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_transformer_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_transformer_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated transformer_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextTransformerRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_schema_id, 'x', sizeof(invalid.source_schema_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_transformer_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_transformer_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_schema_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextTransformerRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_schema_id, 'x', sizeof(invalid.target_schema_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_transformer_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_transformer_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_schema_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextTransformerRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_field, 'x', sizeof(invalid.source_field));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_transformer_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_transformer_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_field was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextTransformerRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_field, 'x', sizeof(invalid.target_field));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_transformer_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_transformer_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_field was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextTransformerRuleTransferCases, UmiContextTransformerRule,
    umi_context_transformer_rule_archive_encode, umi_context_transformer_rule_archive_decode,
    UmiContextTransformerRuleTransferEqual, UmiContextTransformerRuleTransferTails, UmiContextTransformerRuleTransferMalformed)

int main(void)
{
    UmiContextTransformerRule value;
    umi_context_transformer_rule_init(&value);
    value.transformer_id[0] = 's';
    value.source_schema_id[0] = 's';
    value.target_schema_id[0] = 's';
    value.source_field[0] = 's';
    value.target_field[0] = 's';
    value.enabled = true;
    value.revision = (uint64_t)17U;
    if (umi_context_transformer_rule_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiContextTransformerRuleTransferCases(&value) != 0) return 1;

    return 0;
}
