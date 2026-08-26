/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/mosaic/context_link_policy.h
 *
 * PURPOSE:
 *   Define toolkit-neutral context link policy contracts for the Framework-owned workbench mosaic platform.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_MOSAIC_CONTEXT_LINK_POLICY_H
#define UMICOM_UI_MOSAIC_CONTEXT_LINK_POLICY_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/ui/mosaic/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiUiMosaicContextLinkPolicy {
    char group_id[UMI_UI_MOSAIC_ID_CAPACITY];
    char context_type[UMI_UI_MOSAIC_ID_CAPACITY];
    char member_id[UMI_UI_MOSAIC_ID_CAPACITY];
    uint32_t colour_index;
    bool bidirectional;
} UmiUiMosaicContextLinkPolicy;

/* Initializes one typed cross-panel context-link contract. */
void umi_ui_mosaic_context_link_policy_init(UmiUiMosaicContextLinkPolicy *value);
/* Configures group, context type and member identity. */
UmiStatus umi_ui_mosaic_context_link_policy_set(UmiUiMosaicContextLinkPolicy *value, const char *group_id, const char *context_type, const char *member_id);
/* Validates typed link identities before routing context. */
UmiStatus umi_ui_mosaic_context_link_policy_validate(const UmiUiMosaicContextLinkPolicy *value);

#ifdef __cplusplus
}
#endif
#endif
