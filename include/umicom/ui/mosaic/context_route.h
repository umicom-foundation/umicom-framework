/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/context_route.h
 *
 * PURPOSE:
 *   Define toolkit-neutral context route contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_CONTEXT_ROUTE_H
#define UMICOM_UI_MOSAIC_CONTEXT_ROUTE_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic context route data shared with callers of this public contract.
 */
typedef struct UmiUiMosaicContextRoute {
    char group_id[UMI_UI_MOSAIC_ID_CAPACITY];
    char context_type[UMI_UI_MOSAIC_ID_CAPACITY];
    char member_id[UMI_UI_MOSAIC_ID_CAPACITY];
    uint32_t colour_index;
    bool bidirectional;
} UmiUiMosaicContextRoute;

/* Initializes one typed cross-panel context-link contract. */
void umi_ui_mosaic_context_route_init(UmiUiMosaicContextRoute *value);
/* Configures group, context type and member identity. */
UmiStatus umi_ui_mosaic_context_route_set(UmiUiMosaicContextRoute *value, const char *group_id, const char *context_type, const char *member_id);
/* Validates typed link identities before routing context. */
UmiStatus umi_ui_mosaic_context_route_validate(const UmiUiMosaicContextRoute *value);

#ifdef __cplusplus
}
#endif
#endif
