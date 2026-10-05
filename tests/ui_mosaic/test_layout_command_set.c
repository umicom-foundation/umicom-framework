/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_layout_command_set.c
 *
 * PURPOSE:
 *   Exercise layout command set behaviour and invariants.
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
#include "umicom/ui/mosaic/layout_command_set.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/mosaic/layout_command_set.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiMosaicLayoutCommandSetTransferEqual(const UmiUiMosaicLayoutCommandSet *a, const UmiUiMosaicLayoutCommandSet *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->label, b->label) == 0 &&
        a->ordinal == b->ordinal &&
        a->enabled == b->enabled &&
        a->requires_edit_mode == b->requires_edit_mode;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiMosaicLayoutCommandSetTransferTails(UmiUiMosaicLayoutCommandSet *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiMosaicLayoutCommandSetTransferMalformed(const UmiUiMosaicLayoutCommandSet *sample)
{
    (void)sample;
    {
        UmiUiMosaicLayoutCommandSet invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_mosaic_layout_command_set_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_mosaic_layout_command_set_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiMosaicLayoutCommandSet invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_mosaic_layout_command_set_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_mosaic_layout_command_set_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiMosaicLayoutCommandSetTransferCases, UmiUiMosaicLayoutCommandSet,
    umi_ui_mosaic_layout_command_set_archive_encode, umi_ui_mosaic_layout_command_set_archive_decode,
    UmiUiMosaicLayoutCommandSetTransferEqual, UmiUiMosaicLayoutCommandSetTransferTails, UmiUiMosaicLayoutCommandSetTransferMalformed)

int main(void) {
    UmiUiMosaicLayoutCommandSet value;
    umi_ui_mosaic_layout_command_set_init(&value);
    CHECK(umi_ui_mosaic_layout_command_set_set(&value, "layout.command.layout_command_set", "Layout Command Set") == UMI_STATUS_OK);
    value.requires_edit_mode = true;
    CHECK(umi_ui_mosaic_layout_command_set_validate(&value) == UMI_STATUS_OK);
    if (UmiUiMosaicLayoutCommandSetTransferCases(&value) != 0) return 1;

    CHECK(umi_ui_mosaic_layout_command_set_can_execute(&value, UMI_UI_MOSAIC_EDIT_LOCKED) == 0);
    CHECK(umi_ui_mosaic_layout_command_set_can_execute(&value, UMI_UI_MOSAIC_EDIT_UNLOCKED) == 1);
    return 0;
}
