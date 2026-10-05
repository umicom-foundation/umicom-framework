/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_panel.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/panel.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/panel.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCrossApplicationPanelTransferEqual(const UmiCrossApplicationPanel *a, const UmiCrossApplicationPanel *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
        strcmp(a->application_id, b->application_id) == 0 &&
        strcmp(a->title, b->title) == 0 &&
        strcmp(a->component_id, b->component_id) == 0 &&
        strcmp(a->default_channel_id, b->default_channel_id) == 0 &&
        a->singleton == b->singleton &&
        a->context_aware == b->context_aware &&
        a->enabled == b->enabled &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCrossApplicationPanelTransferTails(UmiCrossApplicationPanel *value)
{
    (void)value;
    {
        size_t used = strlen(value->panel_id) + 1U;
        memset(value->panel_id + used, 0xa5, sizeof(value->panel_id) - used);
    }
    {
        size_t used = strlen(value->application_id) + 1U;
        memset(value->application_id + used, 0xa5, sizeof(value->application_id) - used);
    }
    {
        size_t used = strlen(value->title) + 1U;
        memset(value->title + used, 0xa5, sizeof(value->title) - used);
    }
    {
        size_t used = strlen(value->component_id) + 1U;
        memset(value->component_id + used, 0xa5, sizeof(value->component_id) - used);
    }
    {
        size_t used = strlen(value->default_channel_id) + 1U;
        memset(value->default_channel_id + used, 0xa5, sizeof(value->default_channel_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCrossApplicationPanelTransferMalformed(const UmiCrossApplicationPanel *sample)
{
    (void)sample;
    {
        UmiCrossApplicationPanel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_panel_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_panel_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCrossApplicationPanel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_panel_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_panel_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCrossApplicationPanel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.title, 'x', sizeof(invalid.title));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_panel_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_panel_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated title was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCrossApplicationPanel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.component_id, 'x', sizeof(invalid.component_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_panel_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_panel_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated component_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCrossApplicationPanel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.default_channel_id, 'x', sizeof(invalid.default_channel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_panel_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_panel_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated default_channel_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCrossApplicationPanelTransferCases, UmiCrossApplicationPanel,
    umi_context_panel_archive_encode, umi_context_panel_archive_decode,
    UmiCrossApplicationPanelTransferEqual, UmiCrossApplicationPanelTransferTails, UmiCrossApplicationPanelTransferMalformed)

int main(void)
{
    UmiCrossApplicationPanel value;
    umi_context_panel_init(&value);
    value.panel_id[0] = 's';
    value.application_id[0] = 's';
    value.title[0] = 's';
    value.component_id[0] = 's';
    value.default_channel_id[0] = 's';
    value.singleton = true;
    value.context_aware = true;
    value.enabled = true;
    value.revision = (uint64_t)17U;
    if (umi_context_panel_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiCrossApplicationPanelTransferCases(&value) != 0) return 1;

    return 0;
}
