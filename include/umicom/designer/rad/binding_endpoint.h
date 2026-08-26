/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/binding_endpoint.h
 *
 * PURPOSE:
 *   Represent one source or destination property endpoint in the visual binding editor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_BINDING_ENDPOINT_H
#define UMICOM_DESIGNER_RAD_BINDING_ENDPOINT_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadBindingEndpoint {
    char node_id[UMI_RAD_ID_CAPACITY];
    char property_path[UMI_RAD_PATH_CAPACITY];
    bool output;
} UmiRadBindingEndpoint;
UmiStatus umi_rad_binding_endpoint_init(UmiRadBindingEndpoint *item);
int umi_rad_binding_endpoint_is_valid(const UmiRadBindingEndpoint *item);
#ifdef __cplusplus
}
#endif
#endif
