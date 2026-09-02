/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_plugin_capability.c
 *
 * PURPOSE:
 *   Exercise describe one capability requested or exported by an extension.
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
#include "umicom/plugin/extension_host/plugin_capability.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiPluginExtensionHostPluginCapability value; umi_plugin_extension_host_plugin_capability_init(&value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_plugin_extension_host_plugin_capability_configure(&value, "sample.extension", "evidence", 2U, 12U, UINT64_C(3)) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_plugin_extension_host_plugin_capability_validate(&value) != UMI_STATUS_OK) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_plugin_extension_host_plugin_capability_fingerprint(&value) == 0U) return 3;
    return 0;
}
