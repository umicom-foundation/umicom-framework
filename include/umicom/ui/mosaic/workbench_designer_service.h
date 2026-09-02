/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/workbench_designer_service.h
 *
 * PURPOSE:
 *   Aggregate selection, drag, drop-preview and edit-session state for a renderer-neutral Layout Designer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_WORKBENCH_DESIGNER_SERVICE_H
#define UMICOM_UI_MOSAIC_WORKBENCH_DESIGNER_SERVICE_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic workbench designer service data shared with callers of this
 * public contract.
 */
typedef struct UmiUiMosaicWorkbenchDesignerService {
    char workspace_id[UMI_UI_MOSAIC_ID_CAPACITY];
    char active_id[UMI_UI_MOSAIC_ID_CAPACITY];
    uint32_t revision;
    uint32_t selection_count;
    UmiUiMosaicEditMode mode;
    bool valid;
} UmiUiMosaicWorkbenchDesignerService;

/* Initializes renderer-neutral Layout Designer state. */
void umi_ui_mosaic_workbench_designer_service_init(UmiUiMosaicWorkbenchDesignerService *value);
/* Binds the designer state to a workspace and active object. */
UmiStatus umi_ui_mosaic_workbench_designer_service_bind(UmiUiMosaicWorkbenchDesignerService *value, const char *workspace_id, const char *active_id);
/* Validates selection/edit state before a layout mutation is committed. */
UmiStatus umi_ui_mosaic_workbench_designer_service_validate(const UmiUiMosaicWorkbenchDesignerService *value);
/* Advances the designer revision after one accepted operation. */
UmiStatus umi_ui_mosaic_workbench_designer_service_advance(UmiUiMosaicWorkbenchDesignerService *value);

#ifdef __cplusplus
}
#endif
#endif
