/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/context_link_colour.h
 *
 * PURPOSE:
 *   Define toolkit-neutral context link colour contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_CONTEXT_LINK_COLOUR_H
#define UMICOM_UI_MOSAIC_CONTEXT_LINK_COLOUR_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ui mosaic context link colour data shared with callers of this public
 * contract.
 */
typedef struct UmiUiMosaicContextLinkColour {
    char group_id[UMI_UI_MOSAIC_ID_CAPACITY];
    char context_type[UMI_UI_MOSAIC_ID_CAPACITY];
    char member_id[UMI_UI_MOSAIC_ID_CAPACITY];
    uint32_t colour_index;
    bool bidirectional;
} UmiUiMosaicContextLinkColour;

/* Initializes one typed cross-panel context-link contract. */
void umi_ui_mosaic_context_link_colour_init(UmiUiMosaicContextLinkColour *value);
/* Configures group, context type and member identity. */
UmiStatus umi_ui_mosaic_context_link_colour_set(UmiUiMosaicContextLinkColour *value, const char *group_id, const char *context_type, const char *member_id);
/* Validates typed link identities before routing context. */
UmiStatus umi_ui_mosaic_context_link_colour_validate(const UmiUiMosaicContextLinkColour *value);

#ifdef __cplusplus
}
#endif
#endif
