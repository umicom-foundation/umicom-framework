/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/dock_operation.h
 *
 * PURPOSE:
 *   Define toolkit-neutral dock operation contracts for the Framework-owned workbench mosaic platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_DOCK_OPERATION_H
#define UMICOM_UI_MOSAIC_DOCK_OPERATION_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicDockOperation {
    char source_id[UMI_UI_MOSAIC_ID_CAPACITY];
    char target_id[UMI_UI_MOSAIC_ID_CAPACITY];
    UmiUiMosaicDockZone zone;
    uint32_t sequence;
    bool allowed;
} UmiUiMosaicDockOperation;

/* Initializes one dock operation relation. */
void umi_ui_mosaic_dock_operation_init(UmiUiMosaicDockOperation *value);
/* Configures source/target/zone without performing a renderer mutation. */
UmiStatus umi_ui_mosaic_dock_operation_set(UmiUiMosaicDockOperation *value, const char *source_id, const char *target_id, UmiUiMosaicDockZone zone);
/* Rejects malformed or self-referential dock operations. */
UmiStatus umi_ui_mosaic_dock_operation_validate(const UmiUiMosaicDockOperation *value);
/* Returns whether the relation is a centre/tab-style target. */
int umi_ui_mosaic_dock_operation_is_centre(const UmiUiMosaicDockOperation *value);

#ifdef __cplusplus
}
#endif
#endif
