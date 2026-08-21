/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/palette.h
 *
 * PURPOSE:
 *   Define the reusable component palette used to add Framework and cross-
 *   application panels to semantic layouts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_PALETTE_H
#define UMICOM_WORKBENCH_DESIGNER_PALETTE_H

#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchDesignerPaletteItem {
    char item_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char component_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char owner_application_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char label[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    char category[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char description[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    char icon_resource_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char keywords[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    UmiWorkbenchLayoutNodeKind node_kind;
    UmiWorkbenchLayoutDockRegion default_dock;
    bool singleton;
    bool context_link_capable;
    bool enabled;
    uint32_t order;
} UmiWorkbenchDesignerPaletteItem;

typedef struct UmiWorkbenchDesignerPaletteQuery {
    char text[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    char category[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char owner_application_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    bool include_disabled;
} UmiWorkbenchDesignerPaletteQuery;

typedef struct UmiWorkbenchDesignerPalette {
    UmiWorkbenchDesignerPaletteItem items[UMI_WORKBENCH_DESIGNER_MAX_PALETTE_ITEMS];
    size_t count;
    uint64_t revision;
} UmiWorkbenchDesignerPalette;

void umi_workbench_designer_palette_init(UmiWorkbenchDesignerPalette *palette);
UmiStatus umi_workbench_designer_palette_add(UmiWorkbenchDesignerPalette *palette, const UmiWorkbenchDesignerPaletteItem *item);
UmiStatus umi_workbench_designer_palette_seed_framework(UmiWorkbenchDesignerPalette *palette);
const UmiWorkbenchDesignerPaletteItem *umi_workbench_designer_palette_find(const UmiWorkbenchDesignerPalette *palette, const char *item_id);
size_t umi_workbench_designer_palette_query(const UmiWorkbenchDesignerPalette *palette, const UmiWorkbenchDesignerPaletteQuery *query, size_t *out_indices, size_t capacity);
UmiWorkbenchDesignerPaletteQuery umi_workbench_designer_palette_query_default(void);

#ifdef __cplusplus
}
#endif

#endif
