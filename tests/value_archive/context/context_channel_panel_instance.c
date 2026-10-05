/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_panel_instance.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/panel_instance.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/panel_instance.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCrossApplicationPanelInstanceTransferEqual(const UmiCrossApplicationPanelInstance *a, const UmiCrossApplicationPanelInstance *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->instance_id, b->instance_id) == 0 &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
        strcmp(a->application_id, b->application_id) == 0 &&
        strcmp(a->layout_node_id, b->layout_node_id) == 0 &&
        strcmp(a->channel_id, b->channel_id) == 0 &&
        a->visible == b->visible &&
        a->active == b->active &&
        a->last_context_sequence == b->last_context_sequence &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCrossApplicationPanelInstanceTransferTails(UmiCrossApplicationPanelInstance *value)
{
    (void)value;
    {
        size_t used = strlen(value->instance_id) + 1U;
        memset(value->instance_id + used, 0xa5, sizeof(value->instance_id) - used);
    }
    {
        size_t used = strlen(value->panel_id) + 1U;
        memset(value->panel_id + used, 0xa5, sizeof(value->panel_id) - used);
    }
    {
        size_t used = strlen(value->application_id) + 1U;
        memset(value->application_id + used, 0xa5, sizeof(value->application_id) - used);
    }
    {
        size_t used = strlen(value->layout_node_id) + 1U;
        memset(value->layout_node_id + used, 0xa5, sizeof(value->layout_node_id) - used);
    }
    {
        size_t used = strlen(value->channel_id) + 1U;
        memset(value->channel_id + used, 0xa5, sizeof(value->channel_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCrossApplicationPanelInstanceTransferMalformed(const UmiCrossApplicationPanelInstance *sample)
{
    (void)sample;
    {
        UmiCrossApplicationPanelInstance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instance_id, 'x', sizeof(invalid.instance_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_panel_instance_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_panel_instance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instance_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCrossApplicationPanelInstance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_panel_instance_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_panel_instance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCrossApplicationPanelInstance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_panel_instance_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_panel_instance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCrossApplicationPanelInstance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.layout_node_id, 'x', sizeof(invalid.layout_node_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_panel_instance_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_panel_instance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated layout_node_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCrossApplicationPanelInstance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.channel_id, 'x', sizeof(invalid.channel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_panel_instance_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_panel_instance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated channel_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCrossApplicationPanelInstanceTransferCases, UmiCrossApplicationPanelInstance,
    umi_context_panel_instance_archive_encode, umi_context_panel_instance_archive_decode,
    UmiCrossApplicationPanelInstanceTransferEqual, UmiCrossApplicationPanelInstanceTransferTails, UmiCrossApplicationPanelInstanceTransferMalformed)

int main(void)
{
    UmiCrossApplicationPanelInstance value;
    umi_context_panel_instance_init(&value);
    value.instance_id[0] = 's';
    value.panel_id[0] = 's';
    value.application_id[0] = 's';
    value.layout_node_id[0] = 's';
    value.channel_id[0] = 's';
    value.visible = true;
    value.active = true;
    value.last_context_sequence = (uint64_t)17U;
    value.revision = (uint64_t)17U;
    if (umi_context_panel_instance_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiCrossApplicationPanelInstanceTransferCases(&value) != 0) return 1;

    return 0;
}
