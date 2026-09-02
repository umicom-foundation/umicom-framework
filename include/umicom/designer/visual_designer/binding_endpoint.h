/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/binding_endpoint.h
 *
 * PURPOSE:
 *   Represent one source or destination property endpoint in the visual binding editor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_BINDING_ENDPOINT_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_BINDING_ENDPOINT_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer binding endpoint data shared with callers of this public contract.
 */
typedef struct UmiRadBindingEndpoint {
    char node_id[UMI_RAD_ID_CAPACITY];
    char property_path[UMI_RAD_PATH_CAPACITY];
    bool output;
} UmiRadBindingEndpoint;
/**
 * Initialise visual designer binding endpoint from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_binding_endpoint_init(UmiRadBindingEndpoint *item);
/**
 * Check that visual designer binding endpoint satisfies its contract before another service relies on
 * it.
 */
int umi_rad_binding_endpoint_is_valid(const UmiRadBindingEndpoint *item);
#ifdef __cplusplus
}
#endif
#endif
