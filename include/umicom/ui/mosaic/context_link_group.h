/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/context_link_group.h
 *
 * PURPOSE:
 *   Define toolkit-neutral context link group contracts for the Framework-owned workbench mosaic platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_CONTEXT_LINK_GROUP_H
#define UMICOM_UI_MOSAIC_CONTEXT_LINK_GROUP_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicContextLinkGroup {
    char group_id[UMI_UI_MOSAIC_ID_CAPACITY];
    char context_type[UMI_UI_MOSAIC_ID_CAPACITY];
    char member_id[UMI_UI_MOSAIC_ID_CAPACITY];
    uint32_t colour_index;
    bool bidirectional;
} UmiUiMosaicContextLinkGroup;

/* Initializes one typed cross-panel context-link contract. */
void umi_ui_mosaic_context_link_group_init(UmiUiMosaicContextLinkGroup *value);
/* Configures group, context type and member identity. */
UmiStatus umi_ui_mosaic_context_link_group_set(UmiUiMosaicContextLinkGroup *value, const char *group_id, const char *context_type, const char *member_id);
/* Validates typed link identities before routing context. */
UmiStatus umi_ui_mosaic_context_link_group_validate(const UmiUiMosaicContextLinkGroup *value);

#ifdef __cplusplus
}
#endif
#endif
