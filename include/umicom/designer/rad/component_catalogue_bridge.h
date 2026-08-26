/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/component_catalogue_bridge.h
 *
 * PURPOSE:
 *   Map Design System component identifiers to canonical designer component types.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_COMPONENT_CATALOGUE_BRIDGE_H
#define UMICOM_DESIGNER_RAD_COMPONENT_CATALOGUE_BRIDGE_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadComponentCatalogueBridge {
    char design_component_id[UMI_RAD_ID_CAPACITY];
    char designer_type[UMI_RAD_ID_CAPACITY];
    char family[UMI_RAD_ID_CAPACITY];
    bool available;
} UmiRadComponentCatalogueBridge;
UmiStatus umi_rad_component_catalogue_bridge_init(UmiRadComponentCatalogueBridge *item);
int umi_rad_component_catalogue_bridge_is_valid(const UmiRadComponentCatalogueBridge *item);
#ifdef __cplusplus
}
#endif
#endif
