/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/cross_application_panel_context_binding.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/cross_application_panel/context_binding.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/cross_application_panel/context_binding.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPanelContextBindingTransferEqual(const UmiPanelContextBinding *a, const UmiPanelContextBinding *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->binding_id, b->binding_id) == 0 &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
        strcmp(a->instance_id, b->instance_id) == 0 &&
        strcmp(a->channel_id, b->channel_id) == 0 &&
        strcmp(a->schema_id, b->schema_id) == 0 &&
        a->follow == b->follow &&
        a->publish == b->publish &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPanelContextBindingTransferTails(UmiPanelContextBinding *value)
{
    (void)value;
    {
        size_t used = strlen(value->binding_id) + 1U;
        memset(value->binding_id + used, 0xa5, sizeof(value->binding_id) - used);
    }
    {
        size_t used = strlen(value->panel_id) + 1U;
        memset(value->panel_id + used, 0xa5, sizeof(value->panel_id) - used);
    }
    {
        size_t used = strlen(value->instance_id) + 1U;
        memset(value->instance_id + used, 0xa5, sizeof(value->instance_id) - used);
    }
    {
        size_t used = strlen(value->channel_id) + 1U;
        memset(value->channel_id + used, 0xa5, sizeof(value->channel_id) - used);
    }
    {
        size_t used = strlen(value->schema_id) + 1U;
        memset(value->schema_id + used, 0xa5, sizeof(value->schema_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPanelContextBindingTransferMalformed(const UmiPanelContextBinding *sample)
{
    (void)sample;
    {
        UmiPanelContextBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.binding_id, 'x', sizeof(invalid.binding_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_context_binding_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_context_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated binding_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelContextBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_context_binding_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_context_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelContextBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instance_id, 'x', sizeof(invalid.instance_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_context_binding_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_context_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instance_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelContextBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.channel_id, 'x', sizeof(invalid.channel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_context_binding_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_context_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated channel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelContextBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.schema_id, 'x', sizeof(invalid.schema_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_context_binding_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_context_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated schema_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPanelContextBindingTransferCases, UmiPanelContextBinding,
    umi_panel_context_binding_archive_encode, umi_panel_context_binding_archive_decode,
    UmiPanelContextBindingTransferEqual, UmiPanelContextBindingTransferTails, UmiPanelContextBindingTransferMalformed)

int main(void)
{
    UmiPanelContextBinding value;
    umi_panel_context_binding_init(&value);
    value.binding_id[0] = 's';
    value.panel_id[0] = 's';
    value.instance_id[0] = 's';
    value.channel_id[0] = 's';
    value.schema_id[0] = 's';
    value.follow = true;
    value.publish = true;
    value.revision = (uint64_t)17U;
    if (umi_panel_context_binding_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiPanelContextBindingTransferCases(&value) != 0) return 1;

    return 0;
}
