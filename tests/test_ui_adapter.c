/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ui_adapter.c
 *
 * PURPOSE:
 *   Verify frontend-adapter registration and deterministic owned-adapter destruction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static int destroyed = 0;
/*
 * Exercise present and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiStatus present(void *instance, UmiUiApplicationShell *shell) { (void)instance; (void)shell; return UMI_STATUS_OK; }
/*
 * Exercise destroy and return a clear result when the behaviour no longer matches its
 * contract.
 */
static void destroy(void *instance) { (void)instance; destroyed = 1; }
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiUiAdapterRegistry *registry = NULL;
    UmiUiAdapter adapter = {0};
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
