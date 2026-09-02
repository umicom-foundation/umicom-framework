/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/dock_direction.h
 *
 * PURPOSE:
 *   Define toolkit-neutral dock direction contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_DOCK_DIRECTION_H
#define UMICOM_UI_MOSAIC_DOCK_DIRECTION_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic dock direction data shared with callers of this public contract.
 */
typedef struct UmiUiMosaicDockDirection {
    char source_id[UMI_UI_MOSAIC_ID_CAPACITY];
    char target_id[UMI_UI_MOSAIC_ID_CAPACITY];
    UmiUiMosaicDockZone zone;
    uint32_t sequence;
    bool allowed;
} UmiUiMosaicDockDirection;

/* Initializes one dock direction relation. */
void umi_ui_mosaic_dock_direction_init(UmiUiMosaicDockDirection *value);
/* Configures source/target/zone without performing a renderer mutation. */
UmiStatus umi_ui_mosaic_dock_direction_set(UmiUiMosaicDockDirection *value, const char *source_id, const char *target_id, UmiUiMosaicDockZone zone);
/* Rejects malformed or self-referential dock operations. */
UmiStatus umi_ui_mosaic_dock_direction_validate(const UmiUiMosaicDockDirection *value);
/* Returns whether the relation is a centre/tab-style target. */
int umi_ui_mosaic_dock_direction_is_centre(const UmiUiMosaicDockDirection *value);

#ifdef __cplusplus
}
#endif
#endif
