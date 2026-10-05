/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/cross_application_panel_session.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/cross_application_panel/session.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/cross_application_panel/session.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPanelSessionTransferEqual(const UmiPanelSession *a, const UmiPanelSession *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->session_id, b->session_id) == 0 &&
        strcmp(a->workspace_id, b->workspace_id) == 0 &&
        strcmp(a->active_instance_id, b->active_instance_id) == 0 &&
        strcmp(a->layout_id, b->layout_id) == 0 &&
        a->panel_count == b->panel_count &&
        a->clean_shutdown == b->clean_shutdown &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPanelSessionTransferTails(UmiPanelSession *value)
{
    (void)value;
    {
        size_t used = strlen(value->session_id) + 1U;
        memset(value->session_id + used, 0xa5, sizeof(value->session_id) - used);
    }
    {
        size_t used = strlen(value->workspace_id) + 1U;
        memset(value->workspace_id + used, 0xa5, sizeof(value->workspace_id) - used);
    }
    {
        size_t used = strlen(value->active_instance_id) + 1U;
        memset(value->active_instance_id + used, 0xa5, sizeof(value->active_instance_id) - used);
    }
    {
        size_t used = strlen(value->layout_id) + 1U;
        memset(value->layout_id + used, 0xa5, sizeof(value->layout_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPanelSessionTransferMalformed(const UmiPanelSession *sample)
{
    (void)sample;
    {
        UmiPanelSession invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.session_id, 'x', sizeof(invalid.session_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_session_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_session_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated session_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelSession invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.workspace_id, 'x', sizeof(invalid.workspace_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_session_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_session_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated workspace_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelSession invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.active_instance_id, 'x', sizeof(invalid.active_instance_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_session_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_session_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated active_instance_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelSession invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.layout_id, 'x', sizeof(invalid.layout_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_session_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_session_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated layout_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPanelSessionTransferCases, UmiPanelSession,
    umi_panel_session_archive_encode, umi_panel_session_archive_decode,
    UmiPanelSessionTransferEqual, UmiPanelSessionTransferTails, UmiPanelSessionTransferMalformed)

int main(void)
{
    UmiPanelSession value;
    umi_panel_session_init(&value);
    value.session_id[0] = 's';
    value.workspace_id[0] = 's';
    value.active_instance_id[0] = 's';
    value.layout_id[0] = 's';
    value.panel_count = (uint32_t)17U;
    value.clean_shutdown = true;
    value.revision = (uint64_t)17U;
    if (umi_panel_session_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiPanelSessionTransferCases(&value) != 0) return 1;

    return 0;
}
