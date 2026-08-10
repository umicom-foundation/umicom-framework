/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_adapter.c
 *
 * PURPOSE:
 *   Verify frontend-adapter registration and deterministic owned-adapter destruction.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static int destroyed = 0;
static UmiStatus present(void *instance, UmiUiApplicationShell *shell) { (void)instance; (void)shell; return UMI_STATUS_OK; }
static void destroy(void *instance) { (void)instance; destroyed = 1; }
int main(void)
{
    UmiUiAdapterRegistry *registry = NULL;
    UmiUiAdapterV1 adapter = {0};
    adapter.structure_size = (uint32_t)sizeof(adapter);
    adapter.abi_version = UMI_UI_ADAPTER_ABI_VERSION;
    adapter.adapter_id = "test.adapter";
    adapter.display_name = "Test Adapter";
    adapter.present = present;
    adapter.destroy = destroy;
    assert(umi_ui_adapter_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_ui_adapter_registry_register(registry, &adapter) == UMI_STATUS_OK);
    assert(umi_ui_adapter_registry_find(registry, "test.adapter") != NULL);
    umi_ui_adapter_registry_destroy(registry);
    assert(destroyed == 1);
    return EXIT_SUCCESS;
}
