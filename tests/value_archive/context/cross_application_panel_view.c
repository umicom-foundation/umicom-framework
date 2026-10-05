/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/cross_application_panel_view.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/cross_application_panel/view.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/cross_application_panel/view.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPanelViewTransferEqual(const UmiPanelView *a, const UmiPanelView *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->view_id, b->view_id) == 0 &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
        strcmp(a->instance_id, b->instance_id) == 0 &&
        strcmp(a->title, b->title) == 0 &&
        strcmp(a->subtitle, b->subtitle) == 0 &&
        strcmp(a->empty_message, b->empty_message) == 0 &&
        a->visible == b->visible &&
        a->active == b->active &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPanelViewTransferTails(UmiPanelView *value)
{
    (void)value;
    {
        size_t used = strlen(value->view_id) + 1U;
        memset(value->view_id + used, 0xa5, sizeof(value->view_id) - used);
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
        size_t used = strlen(value->title) + 1U;
        memset(value->title + used, 0xa5, sizeof(value->title) - used);
    }
    {
        size_t used = strlen(value->subtitle) + 1U;
        memset(value->subtitle + used, 0xa5, sizeof(value->subtitle) - used);
    }
    {
        size_t used = strlen(value->empty_message) + 1U;
        memset(value->empty_message + used, 0xa5, sizeof(value->empty_message) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPanelViewTransferMalformed(const UmiPanelView *sample)
{
    (void)sample;
    {
        UmiPanelView invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.view_id, 'x', sizeof(invalid.view_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_view_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_view_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated view_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelView invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_view_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_view_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelView invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instance_id, 'x', sizeof(invalid.instance_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_view_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_view_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instance_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelView invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.title, 'x', sizeof(invalid.title));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_view_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_view_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated title was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelView invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.subtitle, 'x', sizeof(invalid.subtitle));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_view_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_view_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated subtitle was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelView invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.empty_message, 'x', sizeof(invalid.empty_message));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_view_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_view_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated empty_message was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPanelViewTransferCases, UmiPanelView,
    umi_panel_view_archive_encode, umi_panel_view_archive_decode,
    UmiPanelViewTransferEqual, UmiPanelViewTransferTails, UmiPanelViewTransferMalformed)

int main(void)
{
    UmiPanelView value;
    umi_panel_view_init(&value);
    value.view_id[0] = 's';
    value.panel_id[0] = 's';
    value.instance_id[0] = 's';
    value.title[0] = 's';
    value.subtitle[0] = 's';
    value.empty_message[0] = 's';
    value.visible = true;
    value.active = true;
    value.revision = (uint64_t)17U;
    if (umi_panel_view_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiPanelViewTransferCases(&value) != 0) return 1;

    return 0;
}
