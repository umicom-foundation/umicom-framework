/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/monitor_fallback.h
 *
 * PURPOSE:
 *   Define toolkit-neutral monitor fallback contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_MONITOR_FALLBACK_H
#define UMICOM_UI_MOSAIC_MONITOR_FALLBACK_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicMonitorFallback {
    char id[UMI_UI_MOSAIC_ID_CAPACITY];
    char name[UMI_UI_MOSAIC_TEXT_CAPACITY];
    uint32_t revision;
    uint32_t item_count;
    bool locked;
} UmiUiMosaicMonitorFallback;

/* Initializes versioned layout state used by monitor fallback. */
void umi_ui_mosaic_monitor_fallback_init(UmiUiMosaicMonitorFallback *value);
/* Assigns the stable layout identity and display name. */
UmiStatus umi_ui_mosaic_monitor_fallback_set(UmiUiMosaicMonitorFallback *value, const char *id, const char *name);
/* Validates identity and bounded layout cardinality. */
UmiStatus umi_ui_mosaic_monitor_fallback_validate(const UmiUiMosaicMonitorFallback *value);
/* Advances the revision after a committed edit. */
UmiStatus umi_ui_mosaic_monitor_fallback_touch(UmiUiMosaicMonitorFallback *value);

#ifdef __cplusplus
}
#endif
#endif
