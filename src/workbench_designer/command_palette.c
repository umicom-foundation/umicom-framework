/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/command_palette.c
 *
 * PURPOSE:
 *   Seed and filter designer actions without coupling command discovery to GTK
 *   menus or toolbar widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/command_palette.h"
#include "internal.h"


void umi_workbench_designer_command_palette_init(
    UmiWorkbenchDesignerCommandPalette *palette)
{
    if (palette == NULL) return;
    (void)memset(palette, 0, sizeof(*palette));
    palette->selected_index = UMI_WORKBENCH_DESIGNER_INDEX_NONE;
}

static UmiStatus command_palette_add(
    UmiWorkbenchDesignerCommandPalette *palette,
    const char *command_id,
    const char *label,
    const char *category,
    const char *description,
    UmiWorkbenchDesignerCommandKind kind,
    uint32_t order)
{
    UmiWorkbenchDesignerCommandPaletteItem *item;
    if (palette->count >= UMI_WORKBENCH_DESIGNER_MAX_COMMANDS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    item = &palette->items[palette->count++];
    (void)memset(item, 0, sizeof(*item));
    if (umi_workbench_designer_copy_text(
            item->command_id, sizeof(item->command_id), command_id) !=
        UMI_STATUS_OK ||
        umi_workbench_designer_copy_text(
            item->label, sizeof(item->label), label) != UMI_STATUS_OK ||
        umi_workbench_designer_copy_text(
            item->category, sizeof(item->category), category) != UMI_STATUS_OK ||
        umi_workbench_designer_copy_text(
            item->description, sizeof(item->description), description) !=
        UMI_STATUS_OK) {
        palette->count -= 1U;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    item->command_kind = kind;
    item->order = order;
    item->enabled = true;
    item->visible = true;
    return UMI_STATUS_OK;
}

static void command_palette_apply_shortcuts(
    UmiWorkbenchDesignerCommandPalette *palette,
    const UmiWorkbenchDesignerKeymap *keymap)
{
    size_t item_index;
    size_t binding_index;
    if (keymap == NULL) return;
    for (item_index = 0U; item_index < palette->count; ++item_index) {
        for (binding_index = 0U; binding_index < keymap->count; ++binding_index) {
            const UmiWorkbenchDesignerKeybinding *binding =
                &keymap->bindings[binding_index];
            if (strcmp(binding->command_id,
                       palette->items[item_index].command_id) == 0) {
                (void)umi_workbench_designer_copy_text(
                    palette->items[item_index].shortcut,
                    sizeof(palette->items[item_index].shortcut),
                    binding->chord);
                break;
            }
        }
    }
}

UmiStatus umi_workbench_designer_command_palette_seed(
    UmiWorkbenchDesignerCommandPalette *palette,
    const UmiWorkbenchDesignerKeymap *keymap)
{
    UmiStatus status;
    if (palette == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_workbench_designer_command_palette_init(palette);
#define ADD_COMMAND(id, label, category, description, kind, order) \
    do { \
        status = command_palette_add( \
            palette, id, label, category, description, kind, order); \
        if (status != UMI_STATUS_OK) return status; \
    } while (0)
    ADD_COMMAND("designer.add-panel", "Add Panel", "Layout",
        "Add a Framework or application panel to the active layout.",
        UMI_WORKBENCH_DESIGNER_COMMAND_ADD_PANEL, 10U);
    ADD_COMMAND("designer.remove", "Remove Selection", "Edit",
        "Remove the selected node and its owned descendants.",
        UMI_WORKBENCH_DESIGNER_COMMAND_REMOVE_NODE, 20U);
    ADD_COMMAND("designer.split-horizontal", "Split Horizontally", "Layout",
        "Create a horizontal semantic split around the selected node.",
        UMI_WORKBENCH_DESIGNER_COMMAND_SPLIT_HORIZONTAL, 30U);
    ADD_COMMAND("designer.split-vertical", "Split Vertically", "Layout",
        "Create a vertical semantic split around the selected node.",
        UMI_WORKBENCH_DESIGNER_COMMAND_SPLIT_VERTICAL, 40U);
    ADD_COMMAND("designer.tab-group", "Create Tab Group", "Layout",
        "Group selected panels behind one semantic tab strip.",
        UMI_WORKBENCH_DESIGNER_COMMAND_CREATE_TAB_GROUP, 50U);
    ADD_COMMAND("designer.float", "Float Selection", "Layout",
        "Move the selected panel into a floating semantic window.",
        UMI_WORKBENCH_DESIGNER_COMMAND_FLOAT_NODE, 60U);
    ADD_COMMAND("designer.undo", "Undo", "Edit",
        "Reverse the most recent committed designer command.",
        UMI_WORKBENCH_DESIGNER_COMMAND_UNDO, 70U);
    ADD_COMMAND("designer.redo", "Redo", "Edit",
        "Reapply the most recently undone designer command.",
        UMI_WORKBENCH_DESIGNER_COMMAND_REDO, 80U);
    ADD_COMMAND("designer.save", "Save Layout", "File",
        "Persist the current semantic layout through the Data Server boundary.",
        UMI_WORKBENCH_DESIGNER_COMMAND_SAVE, 90U);
    ADD_COMMAND("designer.import", "Import Layout", "File",
        "Import a portable .umilayout document after validation.",
        UMI_WORKBENCH_DESIGNER_COMMAND_IMPORT, 100U);
    ADD_COMMAND("designer.export", "Export Layout", "File",
        "Export the active semantic layout as a portable .umilayout document.",
        UMI_WORKBENCH_DESIGNER_COMMAND_EXPORT, 110U);
    ADD_COMMAND("designer.lock", "Lock Layout", "Layout",
        "Prevent structural edits while retaining navigation and inspection.",
        UMI_WORKBENCH_DESIGNER_COMMAND_LOCK_LAYOUT, 120U);
#undef ADD_COMMAND
    command_palette_apply_shortcuts(palette, keymap);
    palette->selected_index = palette->count > 0U ? 0U
        : UMI_WORKBENCH_DESIGNER_INDEX_NONE;
    palette->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_command_palette_filter(
    UmiWorkbenchDesignerCommandPalette *palette,
    const char *query)
{
    size_t index;
    size_t first_visible = UMI_WORKBENCH_DESIGNER_INDEX_NONE;
    if (palette == NULL || query == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_workbench_designer_copy_text(
            palette->query, sizeof(palette->query), query) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (index = 0U; index < palette->count; ++index) {
        UmiWorkbenchDesignerCommandPaletteItem *item = &palette->items[index];
        item->visible = query[0] == '\0' ||
            umi_workbench_designer_text_contains_case_insensitive(
                item->label, query) ||
            umi_workbench_designer_text_contains_case_insensitive(
                item->category, query) ||
            umi_workbench_designer_text_contains_case_insensitive(
                item->description, query) ||
            umi_workbench_designer_text_contains_case_insensitive(
                item->command_id, query);
        if (item->visible && first_visible == UMI_WORKBENCH_DESIGNER_INDEX_NONE) {
            first_visible = index;
        }
    }
    palette->selected_index = first_visible;
    palette->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_command_palette_move_selection(
    UmiWorkbenchDesignerCommandPalette *palette,
    int direction)
{
    size_t index;
    if (palette == NULL || (direction != -1 && direction != 1)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (palette->selected_index == UMI_WORKBENCH_DESIGNER_INDEX_NONE) {
        return UMI_STATUS_NOT_FOUND;
    }
    index = palette->selected_index;
    for (;;) {
        if (direction > 0) {
            index = index + 1U < palette->count ? index + 1U : 0U;
        } else {
            index = index > 0U ? index - 1U : palette->count - 1U;
        }
        if (palette->items[index].visible) break;
        if (index == palette->selected_index) return UMI_STATUS_NOT_FOUND;
    }
    palette->selected_index = index;
    palette->revision += 1U;
    return UMI_STATUS_OK;
}

const UmiWorkbenchDesignerCommandPaletteItem *umi_workbench_designer_command_palette_selected(
    const UmiWorkbenchDesignerCommandPalette *palette)
{
    return palette != NULL && palette->selected_index < palette->count
        ? &palette->items[palette->selected_index] : NULL;
}

size_t umi_workbench_designer_command_palette_visible_count(
    const UmiWorkbenchDesignerCommandPalette *palette)
{
    size_t count = 0U;
    size_t index;
    if (palette == NULL) return 0U;
    for (index = 0U; index < palette->count; ++index) {
        if (palette->items[index].visible) count += 1U;
    }
    return count;
}
