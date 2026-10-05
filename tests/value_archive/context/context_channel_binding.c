/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_binding.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/binding.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/binding.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextBindingTransferEqual(const UmiContextBinding *a, const UmiContextBinding *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->binding_id, b->binding_id) == 0 &&
        strcmp(a->application_id, b->application_id) == 0 &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
        strcmp(a->channel_id, b->channel_id) == 0 &&
        strcmp(a->accepted_schema_id, b->accepted_schema_id) == 0 &&
        a->publish_enabled == b->publish_enabled &&
        a->observe_enabled == b->observe_enabled &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextBindingTransferTails(UmiContextBinding *value)
{
    (void)value;
    {
        size_t used = strlen(value->binding_id) + 1U;
        memset(value->binding_id + used, 0xa5, sizeof(value->binding_id) - used);
    }
    {
        size_t used = strlen(value->application_id) + 1U;
        memset(value->application_id + used, 0xa5, sizeof(value->application_id) - used);
    }
    {
        size_t used = strlen(value->panel_id) + 1U;
        memset(value->panel_id + used, 0xa5, sizeof(value->panel_id) - used);
    }
    {
        size_t used = strlen(value->channel_id) + 1U;
        memset(value->channel_id + used, 0xa5, sizeof(value->channel_id) - used);
    }
    {
        size_t used = strlen(value->accepted_schema_id) + 1U;
        memset(value->accepted_schema_id + used, 0xa5, sizeof(value->accepted_schema_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextBindingTransferMalformed(const UmiContextBinding *sample)
{
    (void)sample;
    {
        UmiContextBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.binding_id, 'x', sizeof(invalid.binding_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_binding_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated binding_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_binding_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_binding_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.channel_id, 'x', sizeof(invalid.channel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_binding_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated channel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.accepted_schema_id, 'x', sizeof(invalid.accepted_schema_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_binding_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated accepted_schema_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextBindingTransferCases, UmiContextBinding,
    umi_context_binding_archive_encode, umi_context_binding_archive_decode,
    UmiContextBindingTransferEqual, UmiContextBindingTransferTails, UmiContextBindingTransferMalformed)

int main(void)
{
    UmiContextBinding value;
    umi_context_binding_init(&value);
    value.binding_id[0] = 's';
    value.application_id[0] = 's';
    value.panel_id[0] = 's';
    value.channel_id[0] = 's';
    value.accepted_schema_id[0] = 's';
    value.publish_enabled = true;
    value.observe_enabled = true;
    value.revision = (uint64_t)17U;
    if (umi_context_binding_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiContextBindingTransferCases(&value) != 0) return 1;

    return 0;
}
