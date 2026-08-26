/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/computed_registry.h
 *
 * PURPOSE:
 *   Store bounded derived-state identifiers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_COMPUTED_REGISTRY_H
#define UMICOM_UI_REACTIVE_COMPUTED_REGISTRY_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveComputedRegistry {
    char ids[128][UMI_UI_REACTIVE_ID_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiUiReactiveComputedRegistry;
void umi_ui_reactive_computed_registry_init(UmiUiReactiveComputedRegistry *item);
int umi_ui_reactive_computed_registry_valid(const UmiUiReactiveComputedRegistry *item);
#ifdef __cplusplus
}
#endif
#endif
