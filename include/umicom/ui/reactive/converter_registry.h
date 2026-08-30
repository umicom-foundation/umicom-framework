/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/converter_registry.h
 *
 * PURPOSE:
 *   Store converter descriptors by stable identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_CONVERTER_REGISTRY_H
#define UMICOM_UI_REACTIVE_CONVERTER_REGISTRY_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveConverterRegistry {
    char ids[64][UMI_UI_REACTIVE_ID_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiUiReactiveConverterRegistry;
void umi_ui_reactive_converter_registry_init(UmiUiReactiveConverterRegistry *item);
int umi_ui_reactive_converter_registry_valid(const UmiUiReactiveConverterRegistry *item);
#ifdef __cplusplus
}
#endif
#endif
