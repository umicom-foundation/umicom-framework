/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_panel_auto_hide_policy.c
 *
 * PURPOSE:
 *   Exercise panel auto hide policy behaviour and invariants.
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
#include "umicom/ui/mosaic/panel_auto_hide_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/mosaic/panel_auto_hide_policy.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiMosaicPanelAutoHidePolicyTransferEqual(const UmiUiMosaicPanelAutoHidePolicy *a, const UmiUiMosaicPanelAutoHidePolicy *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->title, b->title) == 0 &&
        a->application == b->application &&
        a->priority == b->priority &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiMosaicPanelAutoHidePolicyTransferTails(UmiUiMosaicPanelAutoHidePolicy *value)
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
static int UmiUiMosaicPanelAutoHidePolicyTransferMalformed(const UmiUiMosaicPanelAutoHidePolicy *sample)
{
    (void)sample;
    {
        UmiUiMosaicPanelAutoHidePolicy invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_mosaic_panel_auto_hide_policy_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_mosaic_panel_auto_hide_policy_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiMosaicPanelAutoHidePolicy invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.title, 'x', sizeof(invalid.title));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_mosaic_panel_auto_hide_policy_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_mosaic_panel_auto_hide_policy_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated title was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiMosaicPanelAutoHidePolicyTransferCases, UmiUiMosaicPanelAutoHidePolicy,
    umi_ui_mosaic_panel_auto_hide_policy_archive_encode, umi_ui_mosaic_panel_auto_hide_policy_archive_decode,
    UmiUiMosaicPanelAutoHidePolicyTransferEqual, UmiUiMosaicPanelAutoHidePolicyTransferTails, UmiUiMosaicPanelAutoHidePolicyTransferMalformed)

int main(void) {
    UmiUiMosaicPanelAutoHidePolicy value;
    umi_ui_mosaic_panel_auto_hide_policy_init(&value);
    CHECK(umi_ui_mosaic_panel_auto_hide_policy_set(&value, "panel.panel_auto_hide_policy", "Panel Auto Hide Policy") == UMI_STATUS_OK);
    value.application = UMI_UI_MOSAIC_APP_STUDIO;
    value.priority = 10U;
    CHECK(umi_ui_mosaic_panel_auto_hide_policy_validate(&value) == UMI_STATUS_OK);
    if (UmiUiMosaicPanelAutoHidePolicyTransferCases(&value) != 0) return 1;

    CHECK(umi_ui_mosaic_panel_auto_hide_policy_rank(&value, 5U) == 16U);
    return 0;
}
