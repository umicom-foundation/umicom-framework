/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_application_panel_pack.c
 *
 * PURPOSE:
 *   Exercise application panel pack behaviour and invariants.
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
#include "umicom/ui/mosaic/application_panel_pack.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/mosaic/application_panel_pack.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiMosaicApplicationPanelPackTransferEqual(const UmiUiMosaicApplicationPanelPack *a, const UmiUiMosaicApplicationPanelPack *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
        a->application == b->application &&
        a->row == b->row &&
        a->column == b->column &&
        a->row_span == b->row_span &&
        a->column_span == b->column_span &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiMosaicApplicationPanelPackTransferTails(UmiUiMosaicApplicationPanelPack *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->panel_id) + 1U;
        memset(value->panel_id + used, 0xa5, sizeof(value->panel_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiMosaicApplicationPanelPackTransferMalformed(const UmiUiMosaicApplicationPanelPack *sample)
{
    (void)sample;
    {
        UmiUiMosaicApplicationPanelPack invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_mosaic_application_panel_pack_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_mosaic_application_panel_pack_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiMosaicApplicationPanelPack invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_mosaic_application_panel_pack_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_mosaic_application_panel_pack_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiMosaicApplicationPanelPackTransferCases, UmiUiMosaicApplicationPanelPack,
    umi_ui_mosaic_application_panel_pack_archive_encode, umi_ui_mosaic_application_panel_pack_archive_decode,
    UmiUiMosaicApplicationPanelPackTransferEqual, UmiUiMosaicApplicationPanelPackTransferTails, UmiUiMosaicApplicationPanelPackTransferMalformed)

int main(void) {
    UmiUiMosaicApplicationPanelPack value;
    umi_ui_mosaic_application_panel_pack_init(&value);
    CHECK(umi_ui_mosaic_application_panel_pack_place(&value, "cell.application_panel_pack", "studio.editor", UMI_UI_MOSAIC_APP_STUDIO, 1U, 2U) == UMI_STATUS_OK);
    value.row_span = 2U; value.column_span = 3U;
    CHECK(umi_ui_mosaic_application_panel_pack_validate(&value) == UMI_STATUS_OK);
    if (UmiUiMosaicApplicationPanelPackTransferCases(&value) != 0) return 1;

    CHECK(umi_ui_mosaic_application_panel_pack_area(&value) == 6U);
    return 0;
}
