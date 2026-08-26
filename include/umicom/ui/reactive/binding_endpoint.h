/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/binding_endpoint.h
 *
 * PURPOSE:
 *   Describe a view/property endpoint without owning the target object.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_BINDING_ENDPOINT_H
#define UMICOM_UI_REACTIVE_BINDING_ENDPOINT_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveBindingEndpoint {
    char view_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char property_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    bool writable;
} UmiUiReactiveBindingEndpoint;
void umi_ui_reactive_binding_endpoint_init(UmiUiReactiveBindingEndpoint *item);
int umi_ui_reactive_binding_endpoint_valid(const UmiUiReactiveBindingEndpoint *item);
#ifdef __cplusplus
}
#endif
#endif
