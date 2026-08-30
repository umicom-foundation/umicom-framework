/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/types.h
 *
 * PURPOSE:
 *   Define stable types shared by panel catalogues, layout editing, docking and cross-application mosaic workspaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_TYPES_H
#define UMICOM_UI_MOSAIC_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_MOSAIC_ID_CAPACITY 128U
#define UMI_UI_MOSAIC_TEXT_CAPACITY 256U
#define UMI_UI_MOSAIC_MAX_ITEMS 256U
#define UMI_UI_MOSAIC_MAX_CELLS 64U
#define UMI_UI_MOSAIC_MAX_MEMBERS 64U

typedef enum UmiUiMosaicApplication {
    UMI_UI_MOSAIC_APP_FRAMEWORK = 1,
    UMI_UI_MOSAIC_APP_DESK = 2,
    UMI_UI_MOSAIC_APP_STUDIO = 3,
    UMI_UI_MOSAIC_APP_TRADER = 4,
    UMI_UI_MOSAIC_APP_TMS = 5,
    UMI_UI_MOSAIC_APP_BANK = 6,
    UMI_UI_MOSAIC_APP_LLM = 7,
    UMI_UI_MOSAIC_APP_ACCOUNTANT = 8,
    UMI_UI_MOSAIC_APP_COMMODITY = 9,
    UMI_UI_MOSAIC_APP_MUSIC = 10,
    UMI_UI_MOSAIC_APP_VIDEO = 11,
    UMI_UI_MOSAIC_APP_OS = 12
} UmiUiMosaicApplication;

typedef enum UmiUiMosaicDockZone {
    UMI_UI_MOSAIC_DOCK_LEFT = 1,
    UMI_UI_MOSAIC_DOCK_RIGHT = 2,
    UMI_UI_MOSAIC_DOCK_TOP = 3,
    UMI_UI_MOSAIC_DOCK_BOTTOM = 4,
    UMI_UI_MOSAIC_DOCK_CENTRE = 5,
    UMI_UI_MOSAIC_DOCK_FLOAT = 6
} UmiUiMosaicDockZone;

typedef enum UmiUiMosaicPanelState {
    UMI_UI_MOSAIC_PANEL_HIDDEN = 1,
    UMI_UI_MOSAIC_PANEL_DOCKED = 2,
    UMI_UI_MOSAIC_PANEL_TABBED = 3,
    UMI_UI_MOSAIC_PANEL_FLOATING = 4,
    UMI_UI_MOSAIC_PANEL_DETACHED = 5,
    UMI_UI_MOSAIC_PANEL_AUTO_HIDDEN = 6
} UmiUiMosaicPanelState;

typedef enum UmiUiMosaicEditMode {
    UMI_UI_MOSAIC_EDIT_LOCKED = 1,
    UMI_UI_MOSAIC_EDIT_UNLOCKED = 2
} UmiUiMosaicEditMode;

/* Copies user-visible identifiers/text into bounded Framework storage. */
UmiStatus umi_ui_mosaic_copy_text(char *destination, size_t capacity, const char *source);
int umi_ui_mosaic_id_is_valid(const char *identifier);
const char *umi_ui_mosaic_application_name(UmiUiMosaicApplication application);
const char *umi_ui_mosaic_dock_zone_name(UmiUiMosaicDockZone zone);

#ifdef __cplusplus
}
#endif
#endif
