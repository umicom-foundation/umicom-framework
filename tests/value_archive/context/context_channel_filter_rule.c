/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_filter_rule.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/filter_rule.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/filter_rule.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextFilterRuleTransferEqual(const UmiContextFilterRule *a, const UmiContextFilterRule *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->filter_id, b->filter_id) == 0 &&
        strcmp(a->schema_id, b->schema_id) == 0 &&
        strcmp(a->field_name, b->field_name) == 0 &&
        strcmp(a->expected_text, b->expected_text) == 0 &&
        a->invert == b->invert &&
        a->enabled == b->enabled &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextFilterRuleTransferTails(UmiContextFilterRule *value)
{
    (void)value;
    {
        size_t used = strlen(value->filter_id) + 1U;
        memset(value->filter_id + used, 0xa5, sizeof(value->filter_id) - used);
    }
    {
        size_t used = strlen(value->schema_id) + 1U;
        memset(value->schema_id + used, 0xa5, sizeof(value->schema_id) - used);
    }
    {
        size_t used = strlen(value->field_name) + 1U;
        memset(value->field_name + used, 0xa5, sizeof(value->field_name) - used);
    }
    {
        size_t used = strlen(value->expected_text) + 1U;
        memset(value->expected_text + used, 0xa5, sizeof(value->expected_text) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextFilterRuleTransferMalformed(const UmiContextFilterRule *sample)
{
    (void)sample;
    {
        UmiContextFilterRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.filter_id, 'x', sizeof(invalid.filter_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_filter_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_filter_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated filter_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextFilterRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.schema_id, 'x', sizeof(invalid.schema_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_filter_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_filter_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated schema_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextFilterRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.field_name, 'x', sizeof(invalid.field_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_filter_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_filter_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated field_name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextFilterRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.expected_text, 'x', sizeof(invalid.expected_text));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_filter_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_filter_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated expected_text was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextFilterRuleTransferCases, UmiContextFilterRule,
    umi_context_filter_rule_archive_encode, umi_context_filter_rule_archive_decode,
    UmiContextFilterRuleTransferEqual, UmiContextFilterRuleTransferTails, UmiContextFilterRuleTransferMalformed)

int main(void)
{
    UmiContextFilterRule value;
    umi_context_filter_rule_init(&value);
    value.filter_id[0] = 's';
    value.schema_id[0] = 's';
    value.field_name[0] = 's';
    value.expected_text[0] = 's';
    value.invert = true;
    value.enabled = true;
    value.revision = (uint64_t)17U;
    if (umi_context_filter_rule_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiContextFilterRuleTransferCases(&value) != 0) return 1;

    return 0;
}
