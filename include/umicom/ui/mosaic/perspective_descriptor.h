/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/perspective_descriptor.h
 *
 * PURPOSE:
 *   Define toolkit-neutral perspective descriptor contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_PERSPECTIVE_DESCRIPTOR_H
#define UMICOM_UI_MOSAIC_PERSPECTIVE_DESCRIPTOR_H

#include <stdbool.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicPerspectiveDescriptor {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char name[UMI_UI_MOSAIC_TEXT_CAPACITY];
    char layout_id[UMI_UI_MOSAIC_ID_CAPACITY];
    UmiUiMosaicApplication application;
    bool available;
} UmiUiMosaicPerspectiveDescriptor;

/* Initializes a task/workspace descriptor without toolkit state. */
void umi_ui_mosaic_perspective_descriptor_init(UmiUiMosaicPerspectiveDescriptor *value);
/* Configures identity, default layout and application ownership. */
UmiStatus umi_ui_mosaic_perspective_descriptor_set(UmiUiMosaicPerspectiveDescriptor *value, const char *id, const char *name, const char *layout_id, UmiUiMosaicApplication application);
/* Validates that a selectable perspective/workspace has a valid layout. */
UmiStatus umi_ui_mosaic_perspective_descriptor_validate(const UmiUiMosaicPerspectiveDescriptor *value);

#ifdef __cplusplus
}
#endif
#endif
