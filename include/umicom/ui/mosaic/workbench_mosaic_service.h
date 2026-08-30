/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/workbench_mosaic_service.h
 *
 * PURPOSE:
 *   Aggregate panel catalogue, layouts, perspectives, workspaces and context-link groups into one Framework service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_WORKBENCH_MOSAIC_SERVICE_H
#define UMICOM_UI_MOSAIC_WORKBENCH_MOSAIC_SERVICE_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicWorkbenchMosaicService {
    char workspace_id[UMI_UI_MOSAIC_ID_CAPACITY];
    char active_id[UMI_UI_MOSAIC_ID_CAPACITY];
    uint32_t revision;
    uint32_t selection_count;
    UmiUiMosaicEditMode mode;
    bool valid;
} UmiUiMosaicWorkbenchMosaicService;

/* Initializes renderer-neutral Layout Designer state. */
void umi_ui_mosaic_workbench_mosaic_service_init(UmiUiMosaicWorkbenchMosaicService *value);
/* Binds the designer state to a workspace and active object. */
UmiStatus umi_ui_mosaic_workbench_mosaic_service_bind(UmiUiMosaicWorkbenchMosaicService *value, const char *workspace_id, const char *active_id);
/* Validates selection/edit state before a layout mutation is committed. */
UmiStatus umi_ui_mosaic_workbench_mosaic_service_validate(const UmiUiMosaicWorkbenchMosaicService *value);
/* Advances the designer revision after one accepted operation. */
UmiStatus umi_ui_mosaic_workbench_mosaic_service_advance(UmiUiMosaicWorkbenchMosaicService *value);

#ifdef __cplusplus
}
#endif
#endif
