/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adapter.h
 *
 * PURPOSE:
 *   Define a stable frontend-adapter function table and registry for headless, GTK4,
 *   web and future Umicom frontend implementations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ADAPTER_H
#define UMICOM_UI_ADAPTER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_UI_ADAPTER_ABI_VERSION 1U
#define UMI_UI_ADAPTER_MAX 16U
/**
 * Represent the ui application shell data shared with callers of this public contract.
 */
typedef struct UmiUiApplicationShell UmiUiApplicationShell;
/**
 * Represent the ui adapter data shared with callers of this public contract.
 */
typedef struct UmiUiAdapter { uint32_t structure_size; uint32_t abi_version; const char *adapter_id; const char *display_name; void *instance; UmiStatus (*present)(void *instance, UmiUiApplicationShell *shell); UmiStatus (*refresh)(void *instance); UmiStatus (*run)(void *instance, int argc, char **argv, int *out_exit_code); void (*destroy)(void *instance); } UmiUiAdapter;
/**
 * Represent the ui adapter registry data shared with callers of this public contract.
 */
typedef struct UmiUiAdapterRegistry UmiUiAdapterRegistry;
/**
 * Initialise ui adapter registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_adapter_registry_create(UmiUiAdapterRegistry **out_registry);
/**
 * Release or reset state held by ui adapter registry so the same storage can be reused
 * safely.
 */
void umi_ui_adapter_registry_destroy(UmiUiAdapterRegistry *registry);
/**
 * Add ui adapter registry only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ui_adapter_registry_register(UmiUiAdapterRegistry *registry, const UmiUiAdapter *adapter);
/**
 * Remove ui adapter registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_adapter_registry_unregister(UmiUiAdapterRegistry *registry, const char *adapter_id);
/**
 * Find ui adapter registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiUiAdapter *umi_ui_adapter_registry_find(const UmiUiAdapterRegistry *registry, const char *adapter_id);
/**
 * Return the number of records represented by ui adapter registry without changing their
 * state.
 */
size_t umi_ui_adapter_registry_count(const UmiUiAdapterRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
