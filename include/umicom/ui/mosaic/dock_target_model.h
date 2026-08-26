/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/dock_target_model.h
 *
 * PURPOSE:
 *   Define toolkit-neutral dock target model contracts for the Framework-owned workbench mosaic platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_DOCK_TARGET_MODEL_H
#define UMICOM_UI_MOSAIC_DOCK_TARGET_MODEL_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicDockTargetModel {
    char source_id[UMI_UI_MOSAIC_ID_CAPACITY];
    char target_id[UMI_UI_MOSAIC_ID_CAPACITY];
    UmiUiMosaicDockZone zone;
    uint32_t sequence;
    bool allowed;
} UmiUiMosaicDockTargetModel;

/* Initializes one dock target model relation. */
void umi_ui_mosaic_dock_target_model_init(UmiUiMosaicDockTargetModel *value);
/* Configures source/target/zone without performing a renderer mutation. */
UmiStatus umi_ui_mosaic_dock_target_model_set(UmiUiMosaicDockTargetModel *value, const char *source_id, const char *target_id, UmiUiMosaicDockZone zone);
/* Rejects malformed or self-referential dock operations. */
UmiStatus umi_ui_mosaic_dock_target_model_validate(const UmiUiMosaicDockTargetModel *value);
/* Returns whether the relation is a centre/tab-style target. */
int umi_ui_mosaic_dock_target_model_is_centre(const UmiUiMosaicDockTargetModel *value);

#ifdef __cplusplus
}
#endif
#endif
