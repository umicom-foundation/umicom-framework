/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/command_palette.h
 *
 * PURPOSE:
 *   Expose discoverable Layout Designer commands through a filterable, keyboard-
 *   oriented palette model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_COMMAND_PALETTE_H
#define UMICOM_WORKBENCH_DESIGNER_COMMAND_PALETTE_H

#include "umicom/workbench_designer/command.h"
#include "umicom/workbench_designer/keymap.h"

#ifdef __cplusplus
extern "C" {
#endif


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

typedef struct UmiWorkbenchDesignerCommandPalette {
    UmiWorkbenchDesignerCommandPaletteItem items[UMI_WORKBENCH_DESIGNER_MAX_COMMANDS];
    size_t count;
    char query[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    size_t selected_index;
    uint64_t revision;
} UmiWorkbenchDesignerCommandPalette;

void umi_workbench_designer_command_palette_init(
    UmiWorkbenchDesignerCommandPalette *palette);
UmiStatus umi_workbench_designer_command_palette_seed(
    UmiWorkbenchDesignerCommandPalette *palette,
    const UmiWorkbenchDesignerKeymap *keymap);
UmiStatus umi_workbench_designer_command_palette_filter(
    UmiWorkbenchDesignerCommandPalette *palette,
    const char *query);
UmiStatus umi_workbench_designer_command_palette_move_selection(
    UmiWorkbenchDesignerCommandPalette *palette,
    int direction);
const UmiWorkbenchDesignerCommandPaletteItem *umi_workbench_designer_command_palette_selected(
    const UmiWorkbenchDesignerCommandPalette *palette);
size_t umi_workbench_designer_command_palette_visible_count(
    const UmiWorkbenchDesignerCommandPalette *palette);

#ifdef __cplusplus
}
#endif

#endif
