/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/computed_registry.h
 *
 * PURPOSE:
 *   Store bounded derived-state identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_COMPUTED_REGISTRY_H
#define UMICOM_UI_REACTIVE_COMPUTED_REGISTRY_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive computed registry data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveComputedRegistry {
    char ids[128][UMI_UI_REACTIVE_ID_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiUiReactiveComputedRegistry;
/**
 * Initialise ui reactive computed registry from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_computed_registry_init(UmiUiReactiveComputedRegistry *item);
/**
 * Check that ui reactive computed registry satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_computed_registry_valid(const UmiUiReactiveComputedRegistry *item);
#ifdef __cplusplus
}
#endif
#endif
