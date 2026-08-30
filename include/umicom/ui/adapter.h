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
typedef struct UmiUiApplicationShell UmiUiApplicationShell;
typedef struct UmiUiAdapter { uint32_t structure_size; uint32_t abi_version; const char *adapter_id; const char *display_name; void *instance; UmiStatus (*present)(void *instance, UmiUiApplicationShell *shell); UmiStatus (*refresh)(void *instance); UmiStatus (*run)(void *instance, int argc, char **argv, int *out_exit_code); void (*destroy)(void *instance); } UmiUiAdapter;
typedef struct UmiUiAdapterRegistry UmiUiAdapterRegistry;
UmiStatus umi_ui_adapter_registry_create(UmiUiAdapterRegistry **out_registry);
void umi_ui_adapter_registry_destroy(UmiUiAdapterRegistry *registry);
UmiStatus umi_ui_adapter_registry_register(UmiUiAdapterRegistry *registry, const UmiUiAdapter *adapter);
UmiStatus umi_ui_adapter_registry_unregister(UmiUiAdapterRegistry *registry, const char *adapter_id);
const UmiUiAdapter *umi_ui_adapter_registry_find(const UmiUiAdapterRegistry *registry, const char *adapter_id);
size_t umi_ui_adapter_registry_count(const UmiUiAdapterRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
