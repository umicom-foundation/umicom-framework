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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/plugin_capability.h"
int main(void)
{
    UmiPluginExtensionHostPluginCapability value; umi_plugin_extension_host_plugin_capability_init(&value);
    if (umi_plugin_extension_host_plugin_capability_configure(&value, "sample.extension", "evidence", 2U, 12U, UINT64_C(3)) != UMI_STATUS_OK) return 1;
    if (umi_plugin_extension_host_plugin_capability_validate(&value) != UMI_STATUS_OK) return 2;
    if (umi_plugin_extension_host_plugin_capability_fingerprint(&value) == 0U) return 3;
    return 0;
}
