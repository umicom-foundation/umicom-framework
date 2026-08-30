/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/binding_wire.h
 *
 * PURPOSE:
 *   Represent a directed visual binding wire between endpoints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_BINDING_WIRE_H
#define UMICOM_DESIGNER_RAD_BINDING_WIRE_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadBindingWire {
    char wire_id[UMI_RAD_ID_CAPACITY];
    char source_node_id[UMI_RAD_ID_CAPACITY];
    char target_node_id[UMI_RAD_ID_CAPACITY];
    bool enabled;
} UmiRadBindingWire;
UmiStatus umi_rad_binding_wire_init(UmiRadBindingWire *item);
int umi_rad_binding_wire_is_valid(const UmiRadBindingWire *item);
#ifdef __cplusplus
}
#endif
#endif
