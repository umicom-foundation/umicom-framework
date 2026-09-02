/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/binding_wire.h
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
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_BINDING_WIRE_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_BINDING_WIRE_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer binding wire data shared with callers of this public contract.
 */
typedef struct UmiRadBindingWire {
    char wire_id[UMI_RAD_ID_CAPACITY];
    char source_node_id[UMI_RAD_ID_CAPACITY];
    char target_node_id[UMI_RAD_ID_CAPACITY];
    bool enabled;
} UmiRadBindingWire;
/**
 * Initialise visual designer binding wire from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_binding_wire_init(UmiRadBindingWire *item);
/**
 * Check that visual designer binding wire satisfies its contract before another service relies on it.
 */
int umi_rad_binding_wire_is_valid(const UmiRadBindingWire *item);
#ifdef __cplusplus
}
#endif
#endif
