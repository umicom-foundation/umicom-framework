/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_designer_drag.c
 *
 * PURPOSE:
 *   Exercise designer drag behaviour and invariants.
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
#include "umicom/ui/mosaic/designer_drag.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/mosaic/designer_drag.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiMosaicDesignerDragTransferEqual(const UmiUiMosaicDesignerDrag *a, const UmiUiMosaicDesignerDrag *b)
{
    return strcmp(a->workspace_id, b->workspace_id) == 0 &&
        strcmp(a->active_id, b->active_id) == 0 &&
        a->revision == b->revision &&
        a->selection_count == b->selection_count &&
        a->mode == b->mode &&
        a->valid == b->valid;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiMosaicDesignerDragTransferTails(UmiUiMosaicDesignerDrag *value)
{
    (void)value;
    {
        size_t used = strlen(value->workspace_id) + 1U;
        memset(value->workspace_id + used, 0xa5, sizeof(value->workspace_id) - used);
    }
    {
        size_t used = strlen(value->active_id) + 1U;
        memset(value->active_id + used, 0xa5, sizeof(value->active_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiMosaicDesignerDragTransferMalformed(const UmiUiMosaicDesignerDrag *sample)
{
    (void)sample;
    {
        UmiUiMosaicDesignerDrag invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.workspace_id, 'x', sizeof(invalid.workspace_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_mosaic_designer_drag_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_mosaic_designer_drag_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated workspace_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiMosaicDesignerDrag invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.active_id, 'x', sizeof(invalid.active_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_mosaic_designer_drag_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_mosaic_designer_drag_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated active_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiMosaicDesignerDragTransferCases, UmiUiMosaicDesignerDrag,
    umi_ui_mosaic_designer_drag_archive_encode, umi_ui_mosaic_designer_drag_archive_decode,
    UmiUiMosaicDesignerDragTransferEqual, UmiUiMosaicDesignerDragTransferTails, UmiUiMosaicDesignerDragTransferMalformed)

int main(void) {
    UmiUiMosaicDesignerDrag value;
    umi_ui_mosaic_designer_drag_init(&value);
    CHECK(umi_ui_mosaic_designer_drag_bind(&value, "workspace.main", "panel.active") == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_designer_drag_validate(&value) == UMI_STATUS_OK);
    if (UmiUiMosaicDesignerDragTransferCases(&value) != 0) return 1;

    CHECK(umi_ui_mosaic_designer_drag_advance(&value) == UMI_STATUS_PERMISSION_DENIED);
    value.mode = UMI_UI_MOSAIC_EDIT_UNLOCKED;
    CHECK(umi_ui_mosaic_designer_drag_advance(&value) == UMI_STATUS_OK);
    return 0;
}
