/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/command_palette.h
 *
 * PURPOSE:
 *   Expose discoverable Layout Designer commands through a filterable, keyboard-
 *   oriented palette model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_COMMAND_PALETTE_H
#define UMICOM_WORKBENCH_DESIGNER_COMMAND_PALETTE_H

#include "umicom/workbench_designer/command.h"
#include "umicom/workbench_designer/keymap.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer command palette item data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchDesignerCommandPaletteItem {
    char command_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char label[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    char category[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char description[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    char shortcut[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchDesignerCommandKind command_kind;
    uint32_t order;
    bool enabled;
    bool visible;
} UmiWorkbenchDesignerCommandPaletteItem;

/**
 * Represent the workbench designer command palette data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerCommandPalette {
    UmiWorkbenchDesignerCommandPaletteItem items[UMI_WORKBENCH_DESIGNER_MAX_COMMANDS];
    size_t count;
    char query[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    size_t selected_index;
    uint64_t revision;
} UmiWorkbenchDesignerCommandPalette;

/**
 * Initialise workbench designer command palette from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_command_palette_init(
    UmiWorkbenchDesignerCommandPalette *palette);
/**
 * Provide the workbench designer command palette seed operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_command_palette_seed(
    UmiWorkbenchDesignerCommandPalette *palette,
    const UmiWorkbenchDesignerKeymap *keymap);
/**
 * Provide the workbench designer command palette filter operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_command_palette_filter(
    UmiWorkbenchDesignerCommandPalette *palette,
    const char *query);
/**
 * Provide the workbench designer command palette move selection operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_designer_command_palette_move_selection(
    UmiWorkbenchDesignerCommandPalette *palette,
    int direction);
/**
 * Find workbench designer command palette while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchDesignerCommandPaletteItem *umi_workbench_designer_command_palette_selected(
    const UmiWorkbenchDesignerCommandPalette *palette);
/**
 * Return the number of records represented by workbench designer command palette visible
 * without changing their state.
 */
size_t umi_workbench_designer_command_palette_visible_count(
    const UmiWorkbenchDesignerCommandPalette *palette);

#ifdef __cplusplus
}
#endif

#endif
