/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/designer_selection.h
 *
 * PURPOSE:
 *   Define toolkit-neutral designer selection contracts for the Framework-owned workbench mosaic platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_DESIGNER_SELECTION_H
#define UMICOM_UI_MOSAIC_DESIGNER_SELECTION_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicDesignerSelection {
    char workspace_id[UMI_UI_MOSAIC_ID_CAPACITY];
    char active_id[UMI_UI_MOSAIC_ID_CAPACITY];
    uint32_t revision;
    uint32_t selection_count;
    UmiUiMosaicEditMode mode;
    bool valid;
} UmiUiMosaicDesignerSelection;

/* Initializes renderer-neutral Layout Designer state. */
void umi_ui_mosaic_designer_selection_init(UmiUiMosaicDesignerSelection *value);
/* Binds the designer state to a workspace and active object. */
UmiStatus umi_ui_mosaic_designer_selection_bind(UmiUiMosaicDesignerSelection *value, const char *workspace_id, const char *active_id);
/* Validates selection/edit state before a layout mutation is committed. */
UmiStatus umi_ui_mosaic_designer_selection_validate(const UmiUiMosaicDesignerSelection *value);
/* Advances the designer revision after one accepted operation. */
UmiStatus umi_ui_mosaic_designer_selection_advance(UmiUiMosaicDesignerSelection *value);

#ifdef __cplusplus
}
#endif
#endif
