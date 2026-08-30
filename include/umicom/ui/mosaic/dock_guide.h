/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/dock_guide.h
 *
 * PURPOSE:
 *   Define toolkit-neutral dock guide contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_DOCK_GUIDE_H
#define UMICOM_UI_MOSAIC_DOCK_GUIDE_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicDockGuide {
    char source_id[UMI_UI_MOSAIC_ID_CAPACITY];
    char target_id[UMI_UI_MOSAIC_ID_CAPACITY];
    UmiUiMosaicDockZone zone;
    uint32_t sequence;
    bool allowed;
} UmiUiMosaicDockGuide;

/* Initializes one dock guide relation. */
void umi_ui_mosaic_dock_guide_init(UmiUiMosaicDockGuide *value);
/* Configures source/target/zone without performing a renderer mutation. */
UmiStatus umi_ui_mosaic_dock_guide_set(UmiUiMosaicDockGuide *value, const char *source_id, const char *target_id, UmiUiMosaicDockZone zone);
/* Rejects malformed or self-referential dock operations. */
UmiStatus umi_ui_mosaic_dock_guide_validate(const UmiUiMosaicDockGuide *value);
/* Returns whether the relation is a centre/tab-style target. */
int umi_ui_mosaic_dock_guide_is_centre(const UmiUiMosaicDockGuide *value);

#ifdef __cplusplus
}
#endif
#endif
