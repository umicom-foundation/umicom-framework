/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_crash_catalogue.c
 *
 * PURPOSE:
 *   Exercise maintain bounded recent crash evidence per extension.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/crash_catalogue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiPluginExtensionHostCrashCatalogue c; umi_plugin_extension_host_crash_catalogue_init(&c);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_plugin_extension_host_crash_catalogue_add(&c,"a",1U,5U,11U)!=UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_plugin_extension_host_crash_catalogue_add(&c,"b",2U,2U,22U)!=UMI_STATUS_OK) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_plugin_extension_host_crash_catalogue_find(&c,"a")==0 || umi_plugin_extension_host_crash_catalogue_count(&c)!=2U) return 3;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_plugin_extension_host_crash_catalogue_remove(&c,"a")!=UMI_STATUS_OK || umi_plugin_extension_host_crash_catalogue_count(&c)!=1U) return 4;
    return 0;
}
