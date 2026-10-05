/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/cross_application_panel_bookmark.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/cross_application_panel/bookmark.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/cross_application_panel/bookmark.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPanelBookmarkTransferEqual(const UmiPanelBookmark *a, const UmiPanelBookmark *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->bookmark_id, b->bookmark_id) == 0 &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
        strcmp(a->instance_id, b->instance_id) == 0 &&
        strcmp(a->label, b->label) == 0 &&
        strcmp(a->workspace_id, b->workspace_id) == 0 &&
        a->created_at_ms == b->created_at_ms &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPanelBookmarkTransferTails(UmiPanelBookmark *value)
{
    (void)value;
    {
        size_t used = strlen(value->bookmark_id) + 1U;
        memset(value->bookmark_id + used, 0xa5, sizeof(value->bookmark_id) - used);
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
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
    {
        size_t used = strlen(value->workspace_id) + 1U;
        memset(value->workspace_id + used, 0xa5, sizeof(value->workspace_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPanelBookmarkTransferMalformed(const UmiPanelBookmark *sample)
{
    (void)sample;
    {
        UmiPanelBookmark invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.bookmark_id, 'x', sizeof(invalid.bookmark_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_bookmark_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_bookmark_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated bookmark_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelBookmark invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_bookmark_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_bookmark_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelBookmark invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instance_id, 'x', sizeof(invalid.instance_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_bookmark_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_bookmark_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instance_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelBookmark invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_bookmark_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_bookmark_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPanelBookmark invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.workspace_id, 'x', sizeof(invalid.workspace_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_panel_bookmark_validate(&invalid) != UMI_STATUS_OK) ||
            umi_panel_bookmark_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated workspace_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPanelBookmarkTransferCases, UmiPanelBookmark,
    umi_panel_bookmark_archive_encode, umi_panel_bookmark_archive_decode,
    UmiPanelBookmarkTransferEqual, UmiPanelBookmarkTransferTails, UmiPanelBookmarkTransferMalformed)

int main(void)
{
    UmiPanelBookmark value;
    umi_panel_bookmark_init(&value);
    value.bookmark_id[0] = 's';
    value.panel_id[0] = 's';
    value.instance_id[0] = 's';
    value.label[0] = 's';
    value.workspace_id[0] = 's';
    value.created_at_ms = (uint64_t)17U;
    value.revision = (uint64_t)17U;
    if (umi_panel_bookmark_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiPanelBookmarkTransferCases(&value) != 0) return 1;

    return 0;
}
