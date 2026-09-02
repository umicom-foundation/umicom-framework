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
/**
 * Represent the ui reactive validator registry data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveValidatorRegistry {
    char ids[64][UMI_UI_REACTIVE_ID_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiUiReactiveValidatorRegistry;
/**
 * Initialise ui reactive validator registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_ui_reactive_validator_registry_init(UmiUiReactiveValidatorRegistry *item);
/**
 * Check that ui reactive validator registry satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_validator_registry_valid(const UmiUiReactiveValidatorRegistry *item);
#ifdef __cplusplus
}
#endif
#endif
