/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/mosaic_workspace.h
 *
 * PURPOSE:
 *   Describe a mixed-application workspace whose cells can host Studio, Trader, TMS, LLM and other panels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_MOSAIC_WORKSPACE_H
#define UMICOM_UI_MOSAIC_MOSAIC_WORKSPACE_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicMosaicWorkspace {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char panel_id[UMI_UI_MOSAIC_ID_CAPACITY];
    UmiUiMosaicApplication application;
    uint16_t row;
    uint16_t column;
    uint16_t row_span;
    uint16_t column_span;
    bool active;
} UmiUiMosaicMosaicWorkspace;

/* Initializes one mixed-application mosaic record. */
void umi_ui_mosaic_mosaic_workspace_init(UmiUiMosaicMosaicWorkspace *value);
/* Places an application panel into a logical mosaic coordinate. */
UmiStatus umi_ui_mosaic_mosaic_workspace_place(UmiUiMosaicMosaicWorkspace *value, const char *id, const char *panel_id, UmiUiMosaicApplication application, uint16_t row, uint16_t column);
/* Validates identity, ownership and bounded span geometry. */
UmiStatus umi_ui_mosaic_mosaic_workspace_validate(const UmiUiMosaicMosaicWorkspace *value);
/* Returns the number of logical cells covered by the record. */
uint32_t umi_ui_mosaic_mosaic_workspace_area(const UmiUiMosaicMosaicWorkspace *value);

#ifdef __cplusplus
}
#endif
#endif
