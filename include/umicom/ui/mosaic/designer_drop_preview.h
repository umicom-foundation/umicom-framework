/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/designer_drop_preview.h
 *
 * PURPOSE:
 *   Define toolkit-neutral designer drop preview contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_DESIGNER_DROP_PREVIEW_H
#define UMICOM_UI_MOSAIC_DESIGNER_DROP_PREVIEW_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicDesignerDropPreview {
    char workspace_id[UMI_UI_MOSAIC_ID_CAPACITY];
    char active_id[UMI_UI_MOSAIC_ID_CAPACITY];
    uint32_t revision;
    uint32_t selection_count;
    UmiUiMosaicEditMode mode;
    bool valid;
} UmiUiMosaicDesignerDropPreview;

/* Initializes renderer-neutral Layout Designer state. */
void umi_ui_mosaic_designer_drop_preview_init(UmiUiMosaicDesignerDropPreview *value);
/* Binds the designer state to a workspace and active object. */
UmiStatus umi_ui_mosaic_designer_drop_preview_bind(UmiUiMosaicDesignerDropPreview *value, const char *workspace_id, const char *active_id);
/* Validates selection/edit state before a layout mutation is committed. */
UmiStatus umi_ui_mosaic_designer_drop_preview_validate(const UmiUiMosaicDesignerDropPreview *value);
/* Advances the designer revision after one accepted operation. */
UmiStatus umi_ui_mosaic_designer_drop_preview_advance(UmiUiMosaicDesignerDropPreview *value);

#ifdef __cplusplus
}
#endif
#endif
