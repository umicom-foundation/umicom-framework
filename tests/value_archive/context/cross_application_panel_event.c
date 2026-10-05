/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/cross_application_panel_event.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/cross_application_panel/event.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/cross_application_panel/event.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPanelEventTransferEqual(const UmiPanelEvent *a, const UmiPanelEvent *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->event_id, b->event_id) == 0 &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
        strcmp(a->instance_id, b->instance_id) == 0 &&
        strcmp(a->event_type, b->event_type) == 0 &&
        strcmp(a->context_id, b->context_id) == 0 &&
        a->timestamp_ms == b->timestamp_ms &&
        a->status == b->status &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPanelEventTransferTails(UmiPanelEvent *value)
{
    (void)value;
    {
        size_t used = strlen(value->event_id) + 1U;
        memset(value->event_id + used, 0xa5, sizeof(value->event_id) - used);
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
        size_t used = strlen(value->event_type) + 1U;
        memset(value->event_type + used, 0xa5, sizeof(value->event_type) - used);
    }
    {
        size_t used = strlen(value->context_id) + 1U;
        memset(value->context_id + used, 0xa5, sizeof(value->context_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPanelEventTransferMalformed(const UmiPanelEvent *sample)
{
    (void)sample;
    {
        UmiPanelEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.event_id, 'x', sizeof(invalid.event_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_event_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated event_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_event_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instance_id, 'x', sizeof(invalid.instance_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_event_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instance_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.event_type, 'x', sizeof(invalid.event_type));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_event_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated event_type was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.context_id, 'x', sizeof(invalid.context_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_event_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated context_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPanelEventTransferCases, UmiPanelEvent,
    umi_panel_event_archive_encode, umi_panel_event_archive_decode,
    UmiPanelEventTransferEqual, UmiPanelEventTransferTails, UmiPanelEventTransferMalformed)

int main(void)
{
    UmiPanelEvent value;
    umi_panel_event_init(&value);
    value.event_id[0] = 's';
    value.panel_id[0] = 's';
    value.instance_id[0] = 's';
    value.event_type[0] = 's';
    value.context_id[0] = 's';
    value.timestamp_ms = (uint64_t)17U;
    value.revision = (uint64_t)17U;
    if (umi_panel_event_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiPanelEventTransferCases(&value) != 0) return 1;

    return 0;
}
