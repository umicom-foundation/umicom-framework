/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/component_catalogue_bridge.h
 *
 * PURPOSE:
 *   Map Design System component identifiers to canonical designer component types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_COMPONENT_CATALOGUE_BRIDGE_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_COMPONENT_CATALOGUE_BRIDGE_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer component catalogue bridge data shared with callers of this public
 * contract.
 */
typedef struct UmiRadComponentCatalogueBridge {
    char design_component_id[UMI_RAD_ID_CAPACITY];
    char designer_type[UMI_RAD_ID_CAPACITY];
    char family[UMI_RAD_ID_CAPACITY];
    bool available;
} UmiRadComponentCatalogueBridge;
/**
 * Initialise visual designer component catalogue bridge from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_rad_component_catalogue_bridge_init(UmiRadComponentCatalogueBridge *item);
/**
 * Check that visual designer component catalogue bridge satisfies its contract before another service
 * relies on it.
 */
int umi_rad_component_catalogue_bridge_is_valid(const UmiRadComponentCatalogueBridge *item);
#ifdef __cplusplus
}
#endif
#endif
