/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/validator_registry.h
 *
 * PURPOSE:
 *   Store validator identifiers for form and property binding pipelines.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_VALIDATOR_REGISTRY_H
#define UMICOM_UI_REACTIVE_VALIDATOR_REGISTRY_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveValidatorRegistry {
    char ids[64][UMI_UI_REACTIVE_ID_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiUiReactiveValidatorRegistry;
void umi_ui_reactive_validator_registry_init(UmiUiReactiveValidatorRegistry *item);
int umi_ui_reactive_validator_registry_valid(const UmiUiReactiveValidatorRegistry *item);
#ifdef __cplusplus
}
#endif
#endif
