/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/palette.c
 *
 * PURPOSE:
 *   Implement palette registration and deterministic search across Framework and
 *   application panel contributions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/palette.h"
#include "internal.h"


void umi_workbench_designer_palette_init(UmiWorkbenchDesignerPalette *palette)
{
    if (palette == NULL) return;
    (void)memset(palette, 0, sizeof(*palette));
}

UmiWorkbenchDesignerPaletteQuery umi_workbench_designer_palette_query_default(void)
{
    UmiWorkbenchDesignerPaletteQuery query;
    (void)memset(&query, 0, sizeof(query));
    return query;
}

const UmiWorkbenchDesignerPaletteItem *umi_workbench_designer_palette_find(
    const UmiWorkbenchDesignerPalette *palette,
    const char *item_id)
{
    size_t index;
    if (palette == NULL || item_id == NULL) return NULL;
    for (index = 0U; index < palette->count; ++index) {
        if (strcmp(palette->items[index].item_id, item_id) == 0) return &palette->items[index];
    }
    return NULL;
}

UmiStatus umi_workbench_designer_palette_add(
    UmiWorkbenchDesignerPalette *palette,
    const UmiWorkbenchDesignerPaletteItem *item)
{
    if (palette == NULL || item == NULL || item->item_id[0] == '\0' ||
        item->component_id[0] == '\0' || item->label[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_workbench_designer_palette_find(palette, item->item_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (palette->count >= UMI_WORKBENCH_DESIGNER_MAX_PALETTE_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    palette->items[palette->count++] = *item;
    palette->revision += 1U;
    return UMI_STATUS_OK;
}

static UmiStatus palette_add_builtin(
    UmiWorkbenchDesignerPalette *palette,
    const char *item_id,
    const char *component_id,
    const char *owner,
    const char *label,
    const char *category,
    const char *description,
    const char *icon,
    const char *keywords,
    UmiWorkbenchLayoutNodeKind kind,
    UmiWorkbenchLayoutDockRegion dock,
    bool singleton,
    bool link_capable,
    uint32_t order)
{
    UmiWorkbenchDesignerPaletteItem item;
    (void)memset(&item, 0, sizeof(item));
    if (umi_workbench_designer_copy_text(item.item_id, sizeof(item.item_id), item_id) != UMI_STATUS_OK ||
        umi_workbench_designer_copy_text(item.component_id, sizeof(item.component_id), component_id) != UMI_STATUS_OK ||
        umi_workbench_designer_copy_text(item.owner_application_id, sizeof(item.owner_application_id), owner) != UMI_STATUS_OK ||
        umi_workbench_designer_copy_text(item.label, sizeof(item.label), label) != UMI_STATUS_OK ||
        umi_workbench_designer_copy_text(item.category, sizeof(item.category), category) != UMI_STATUS_OK ||
        umi_workbench_designer_copy_text(item.description, sizeof(item.description), description) != UMI_STATUS_OK ||
        umi_workbench_designer_copy_text(item.icon_resource_id, sizeof(item.icon_resource_id), icon) != UMI_STATUS_OK ||
        umi_workbench_designer_copy_text(item.keywords, sizeof(item.keywords), keywords) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    item.node_kind = kind;
    item.default_dock = dock;
    item.singleton = singleton;
    item.context_link_capable = link_capable;
    item.enabled = true;
    item.order = order;
    return umi_workbench_designer_palette_add(palette, &item);
}

UmiStatus umi_workbench_designer_palette_seed_framework(
    UmiWorkbenchDesignerPalette *palette)
{
    UmiStatus status;
    if (palette == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = palette_add_builtin(palette, "project-explorer", "umicom.studio.project-explorer", "org.umicom.studio", "Project Explorer", "development", "Browse projects, folders and files.", "umicom.icon.application.studio", "project files explorer workspace", UMI_WORKBENCH_LAYOUT_NODE_PANEL, UMI_WORKBENCH_LAYOUT_DOCK_LEFT, true, true, 10U);
    if (status == UMI_STATUS_OK) status = palette_add_builtin(palette, "editor", "umicom.studio.editor", "org.umicom.studio", "Editor", "development", "Edit source and text documents.", "umicom.icon.application.studio", "editor source document code", UMI_WORKBENCH_LAYOUT_NODE_EDITOR_GROUP, UMI_WORKBENCH_LAYOUT_DOCK_DOCUMENT, false, true, 20U);
    if (status == UMI_STATUS_OK) status = palette_add_builtin(palette, "problems", "umicom.studio.problems", "org.umicom.studio", "Problems", "development", "Inspect diagnostics from compilers and language services.", "umicom.icon.status.warning", "problems diagnostics errors warnings", UMI_WORKBENCH_LAYOUT_NODE_PANEL, UMI_WORKBENCH_LAYOUT_DOCK_BOTTOM, true, true, 30U);
    if (status == UMI_STATUS_OK) status = palette_add_builtin(palette, "output", "umicom.studio.output", "org.umicom.studio", "Output", "development", "Inspect build, test and task output channels.", "umicom.icon.status.information", "output build test log", UMI_WORKBENCH_LAYOUT_NODE_PANEL, UMI_WORKBENCH_LAYOUT_DOCK_BOTTOM, true, true, 40U);
    if (status == UMI_STATUS_OK) status = palette_add_builtin(palette, "terminal", "umicom.studio.terminal", "org.umicom.studio", "Terminal", "development", "Run supervised interactive terminal sessions.", "umicom.icon.action.run", "terminal shell command", UMI_WORKBENCH_LAYOUT_NODE_PANEL, UMI_WORKBENCH_LAYOUT_DOCK_BOTTOM, false, false, 50U);
    if (status == UMI_STATUS_OK) status = palette_add_builtin(palette, "ai-chat", "umicom.ai.chat", "org.umicom.llm", "AI Chat", "ai", "Use governed project-aware AI assistance.", "umicom.icon.application.ai", "ai chat assistant teacher", UMI_WORKBENCH_LAYOUT_NODE_PANEL, UMI_WORKBENCH_LAYOUT_DOCK_RIGHT, false, true, 60U);
    if (status == UMI_STATUS_OK) status = palette_add_builtin(palette, "watchlist", "umicom.trader.watchlist", "org.umicom.trader", "Watchlist", "trading", "Monitor instruments and publish typed instrument context.", "umicom.icon.application.trader", "trader watchlist instrument quote", UMI_WORKBENCH_LAYOUT_NODE_PANEL, UMI_WORKBENCH_LAYOUT_DOCK_LEFT, false, true, 70U);
    if (status == UMI_STATUS_OK) status = palette_add_builtin(palette, "chart", "umicom.trader.chart", "org.umicom.trader", "Chart", "trading", "Display toolkit-neutral market series and studies.", "umicom.icon.application.trader", "trader chart price indicator", UMI_WORKBENCH_LAYOUT_NODE_PANEL, UMI_WORKBENCH_LAYOUT_DOCK_DOCUMENT, false, true, 80U);
    if (status == UMI_STATUS_OK) status = palette_add_builtin(palette, "system-overview", "umicom.os.system-overview", "org.umicom.os", "System Overview", "system", "Inspect system identity, services and health.", "umicom.icon.application.os", "system os health services", UMI_WORKBENCH_LAYOUT_NODE_PANEL, UMI_WORKBENCH_LAYOUT_DOCK_RIGHT, true, false, 90U);
    return status;
}

static bool palette_matches(
    const UmiWorkbenchDesignerPaletteItem *item,
    const UmiWorkbenchDesignerPaletteQuery *query)
{
    bool text_match;
    if (!query->include_disabled && !item->enabled) return false;
    if (query->category[0] != '\0' && strcmp(item->category, query->category) != 0) return false;
    if (query->owner_application_id[0] != '\0' &&
        strcmp(item->owner_application_id, query->owner_application_id) != 0) return false;
    if (query->text[0] == '\0') return true;
    text_match = umi_workbench_designer_text_contains_case_insensitive(item->label, query->text) ||
        umi_workbench_designer_text_contains_case_insensitive(item->description, query->text) ||
        umi_workbench_designer_text_contains_case_insensitive(item->keywords, query->text) ||
        umi_workbench_designer_text_contains_case_insensitive(item->component_id, query->text);
    return text_match;
}

size_t umi_workbench_designer_palette_query(
    const UmiWorkbenchDesignerPalette *palette,
    const UmiWorkbenchDesignerPaletteQuery *query,
    size_t *out_indices,
    size_t capacity)
{
    size_t index;
    size_t count = 0U;
    if (palette == NULL || query == NULL || (capacity > 0U && out_indices == NULL)) return 0U;
    for (index = 0U; index < palette->count; ++index) {
        if (!palette_matches(&palette->items[index], query)) continue;
        if (count < capacity) out_indices[count] = index;
        count += 1U;
    }
    return count;
}
