/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/palette.h
 *
 * PURPOSE:
 *   Define the reusable component palette used to add Framework and cross-
 *   application panels to semantic layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_PALETTE_H
#define UMICOM_WORKBENCH_DESIGNER_PALETTE_H

#include "umicom/workbench_designer/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer palette item data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the workbench designer palette query data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerPaletteQuery {
    char text[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    char category[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char owner_application_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    bool include_disabled;
} UmiWorkbenchDesignerPaletteQuery;

/**
 * Represent the workbench designer palette data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerPalette {
    UmiWorkbenchDesignerPaletteItem items[UMI_WORKBENCH_DESIGNER_MAX_PALETTE_ITEMS];
    size_t count;
    uint64_t revision;
} UmiWorkbenchDesignerPalette;

/**
 * Initialise workbench designer palette from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_palette_init(UmiWorkbenchDesignerPalette *palette);
/**
 * Add workbench designer palette only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_workbench_designer_palette_add(UmiWorkbenchDesignerPalette *palette, const UmiWorkbenchDesignerPaletteItem *item);
/**
 * Provide the workbench designer palette seed framework operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_palette_seed_framework(UmiWorkbenchDesignerPalette *palette);
/**
 * Find workbench designer palette while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchDesignerPaletteItem *umi_workbench_designer_palette_find(const UmiWorkbenchDesignerPalette *palette, const char *item_id);
/**
 * Provide the workbench designer palette query operation used by this module and its
 * client applications.
 */
size_t umi_workbench_designer_palette_query(const UmiWorkbenchDesignerPalette *palette, const UmiWorkbenchDesignerPaletteQuery *query, size_t *out_indices, size_t capacity);
/**
 * Provide the workbench designer palette query default operation used by this module and
 * its client applications.
 */
UmiWorkbenchDesignerPaletteQuery umi_workbench_designer_palette_query_default(void);

#ifdef __cplusplus
}
#endif

#endif
