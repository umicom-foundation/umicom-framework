/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/perspective_switch.h
 *
 * PURPOSE:
 *   Define toolkit-neutral perspective switch contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_PERSPECTIVE_SWITCH_H
#define UMICOM_UI_MOSAIC_PERSPECTIVE_SWITCH_H

#include <stdbool.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicPerspectiveSwitch {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char name[UMI_UI_MOSAIC_TEXT_CAPACITY];
    char layout_id[UMI_UI_MOSAIC_ID_CAPACITY];
    UmiUiMosaicApplication application;
    bool available;
} UmiUiMosaicPerspectiveSwitch;

/* Initializes a task/workspace descriptor without toolkit state. */
void umi_ui_mosaic_perspective_switch_init(UmiUiMosaicPerspectiveSwitch *value);
/* Configures identity, default layout and application ownership. */
UmiStatus umi_ui_mosaic_perspective_switch_set(UmiUiMosaicPerspectiveSwitch *value, const char *id, const char *name, const char *layout_id, UmiUiMosaicApplication application);
/* Validates that a selectable perspective/workspace has a valid layout. */
UmiStatus umi_ui_mosaic_perspective_switch_validate(const UmiUiMosaicPerspectiveSwitch *value);

#ifdef __cplusplus
}
#endif
#endif
