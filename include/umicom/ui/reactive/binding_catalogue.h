/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/binding_catalogue.h
 *
 * PURPOSE:
 *   Store a bounded catalogue of binding identifiers and revisions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_BINDING_CATALOGUE_H
#define UMICOM_UI_REACTIVE_BINDING_CATALOGUE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveBindingCatalogue {
    char ids[UMI_UI_REACTIVE_MAX_ITEMS][UMI_UI_REACTIVE_ID_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiUiReactiveBindingCatalogue;
void umi_ui_reactive_binding_catalogue_init(UmiUiReactiveBindingCatalogue *item);
UmiStatus umi_ui_reactive_binding_catalogue_add(UmiUiReactiveBindingCatalogue *item,const char *id);
int umi_ui_reactive_binding_catalogue_contains(const UmiUiReactiveBindingCatalogue *item,const char *id);
#ifdef __cplusplus
}
#endif
#endif
