/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_policy_rule.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/policy_rule.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/policy_rule.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextPolicyRuleTransferEqual(const UmiContextPolicyRule *a, const UmiContextPolicyRule *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->rule_id, b->rule_id) == 0 &&
        strcmp(a->schema_id, b->schema_id) == 0 &&
        strcmp(a->source_application_id, b->source_application_id) == 0 &&
        strcmp(a->target_application_id, b->target_application_id) == 0 &&
        a->decision == b->decision &&
        a->priority == b->priority &&
        a->enabled == b->enabled &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextPolicyRuleTransferTails(UmiContextPolicyRule *value)
{
    (void)value;
    {
        size_t used = strlen(value->rule_id) + 1U;
        memset(value->rule_id + used, 0xa5, sizeof(value->rule_id) - used);
    }
    {
        size_t used = strlen(value->schema_id) + 1U;
        memset(value->schema_id + used, 0xa5, sizeof(value->schema_id) - used);
    }
    {
        size_t used = strlen(value->source_application_id) + 1U;
        memset(value->source_application_id + used, 0xa5, sizeof(value->source_application_id) - used);
    }
    {
        size_t used = strlen(value->target_application_id) + 1U;
        memset(value->target_application_id + used, 0xa5, sizeof(value->target_application_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextPolicyRuleTransferMalformed(const UmiContextPolicyRule *sample)
{
    (void)sample;
    {
        UmiContextPolicyRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.rule_id, 'x', sizeof(invalid.rule_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_policy_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_policy_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated rule_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextPolicyRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.schema_id, 'x', sizeof(invalid.schema_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_policy_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_policy_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated schema_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextPolicyRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_application_id, 'x', sizeof(invalid.source_application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_policy_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_policy_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_application_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextPolicyRule invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_application_id, 'x', sizeof(invalid.target_application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_policy_rule_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_policy_rule_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_application_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextPolicyRuleTransferCases, UmiContextPolicyRule,
    umi_context_policy_rule_archive_encode, umi_context_policy_rule_archive_decode,
    UmiContextPolicyRuleTransferEqual, UmiContextPolicyRuleTransferTails, UmiContextPolicyRuleTransferMalformed)

int main(void)
{
    UmiContextPolicyRule value;
    umi_context_policy_rule_init(&value);
    value.rule_id[0] = 's';
    value.schema_id[0] = 's';
    value.source_application_id[0] = 's';
    value.target_application_id[0] = 's';
    value.priority = (uint32_t)17U;
    value.enabled = true;
    value.revision = (uint64_t)17U;
    if (umi_context_policy_rule_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiContextPolicyRuleTransferCases(&value) != 0) return 1;

    return 0;
}
