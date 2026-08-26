/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/component_instance.h
 *
 * PURPOSE:
 *   Represent one semantic component instance on a designer document.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_COMPONENT_INSTANCE_H
#define UMICOM_DESIGNER_RAD_COMPONENT_INSTANCE_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadComponentInstance {
    char component_id[UMI_RAD_ID_CAPACITY];
    char component_type[UMI_RAD_ID_CAPACITY];
    char parent_id[UMI_RAD_ID_CAPACITY];
    UmiRadRect bounds;
    bool visible;
} UmiRadComponentInstance;
UmiStatus umi_rad_component_instance_init(UmiRadComponentInstance *item);
int umi_rad_component_instance_is_valid(const UmiRadComponentInstance *item);
#ifdef __cplusplus
}
#endif
#endif
