/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_panel_recent.c
 *
 * PURPOSE:
 *   Exercise panel recent behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/ui/mosaic/panel_recent.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/mosaic/panel_recent.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiMosaicPanelRecentTransferEqual(const UmiUiMosaicPanelRecent *a, const UmiUiMosaicPanelRecent *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->title, b->title) == 0 &&
        a->application == b->application &&
        a->priority == b->priority &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiMosaicPanelRecentTransferTails(UmiUiMosaicPanelRecent *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->title) + 1U;
        memset(value->title + used, 0xa5, sizeof(value->title) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiMosaicPanelRecentTransferMalformed(const UmiUiMosaicPanelRecent *sample)
{
    (void)sample;
    {
        UmiUiMosaicPanelRecent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_mosaic_panel_recent_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_mosaic_panel_recent_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiMosaicPanelRecent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.title, 'x', sizeof(invalid.title));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_mosaic_panel_recent_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_mosaic_panel_recent_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated title was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiMosaicPanelRecentTransferCases, UmiUiMosaicPanelRecent,
    umi_ui_mosaic_panel_recent_archive_encode, umi_ui_mosaic_panel_recent_archive_decode,
    UmiUiMosaicPanelRecentTransferEqual, UmiUiMosaicPanelRecentTransferTails, UmiUiMosaicPanelRecentTransferMalformed)

int main(void) {
    UmiUiMosaicPanelRecent value;
    umi_ui_mosaic_panel_recent_init(&value);
    CHECK(umi_ui_mosaic_panel_recent_set(&value, "panel.panel_recent", "Panel Recent") == UMI_STATUS_OK);
    value.application = UMI_UI_MOSAIC_APP_STUDIO;
    value.priority = 10U;
    CHECK(umi_ui_mosaic_panel_recent_validate(&value) == UMI_STATUS_OK);
    if (UmiUiMosaicPanelRecentTransferCases(&value) != 0) return 1;

    CHECK(umi_ui_mosaic_panel_recent_rank(&value, 5U) == 16U);
    return 0;
}
