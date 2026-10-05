/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/cross_application_panel_focus_state.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/cross_application_panel/focus_state.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/cross_application_panel/focus_state.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPanelFocusStateTransferEqual(const UmiPanelFocusState *a, const UmiPanelFocusState *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->instance_id, b->instance_id) == 0 &&
        strcmp(a->previous_instance_id, b->previous_instance_id) == 0 &&
        a->reason == b->reason &&
        a->focused == b->focused &&
        a->timestamp_ms == b->timestamp_ms &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPanelFocusStateTransferTails(UmiPanelFocusState *value)
{
    (void)value;
    {
        size_t used = strlen(value->instance_id) + 1U;
        memset(value->instance_id + used, 0xa5, sizeof(value->instance_id) - used);
    }
    {
        size_t used = strlen(value->previous_instance_id) + 1U;
        memset(value->previous_instance_id + used, 0xa5, sizeof(value->previous_instance_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPanelFocusStateTransferMalformed(const UmiPanelFocusState *sample)
{
    (void)sample;
    {
        UmiPanelFocusState invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instance_id, 'x', sizeof(invalid.instance_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_focus_state_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_focus_state_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instance_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelFocusState invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.previous_instance_id, 'x', sizeof(invalid.previous_instance_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_focus_state_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_focus_state_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated previous_instance_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPanelFocusStateTransferCases, UmiPanelFocusState,
    umi_panel_focus_state_archive_encode, umi_panel_focus_state_archive_decode,
    UmiPanelFocusStateTransferEqual, UmiPanelFocusStateTransferTails, UmiPanelFocusStateTransferMalformed)

int main(void)
{
    UmiPanelFocusState value;
    umi_panel_focus_state_init(&value);
    value.instance_id[0] = 's';
    value.previous_instance_id[0] = 's';
    value.reason = (uint32_t)17U;
    value.focused = true;
    value.timestamp_ms = (uint64_t)17U;
    value.revision = (uint64_t)17U;
    if (umi_panel_focus_state_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiPanelFocusStateTransferCases(&value) != 0) return 1;

    return 0;
}
