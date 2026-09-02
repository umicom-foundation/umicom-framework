/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/designer_drag.h
 *
 * PURPOSE:
 *   Define toolkit-neutral designer drag contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_DESIGNER_DRAG_H
#define UMICOM_UI_MOSAIC_DESIGNER_DRAG_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic designer drag data shared with callers of this public contract.
 */
typedef struct UmiUiMosaicDesignerDrag {
    char workspace_id[UMI_UI_MOSAIC_ID_CAPACITY];
    char active_id[UMI_UI_MOSAIC_ID_CAPACITY];
    uint32_t revision;
    uint32_t selection_count;
    UmiUiMosaicEditMode mode;
    bool valid;
} UmiUiMosaicDesignerDrag;

/* Initializes renderer-neutral Layout Designer state. */
void umi_ui_mosaic_designer_drag_init(UmiUiMosaicDesignerDrag *value);
/* Binds the designer state to a workspace and active object. */
UmiStatus umi_ui_mosaic_designer_drag_bind(UmiUiMosaicDesignerDrag *value, const char *workspace_id, const char *active_id);
/* Validates selection/edit state before a layout mutation is committed. */
UmiStatus umi_ui_mosaic_designer_drag_validate(const UmiUiMosaicDesignerDrag *value);
/* Advances the designer revision after one accepted operation. */
UmiStatus umi_ui_mosaic_designer_drag_advance(UmiUiMosaicDesignerDrag *value);

#ifdef __cplusplus
}
#endif
#endif
