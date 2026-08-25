/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_abi_descriptor.c
 *
 * PURPOSE:
 *   Exercise describe the stable C ABI contract expected by one extension and its host.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/abi_descriptor.h"
int main(void)
{
    UmiPluginExtensionHostAbiDescriptor value; umi_plugin_extension_host_abi_descriptor_init(&value);
    if (umi_plugin_extension_host_abi_descriptor_configure(&value, "sample.extension", "evidence", 2U, 12U, UINT64_C(3)) != UMI_STATUS_OK) return 1;
    if (umi_plugin_extension_host_abi_descriptor_validate(&value) != UMI_STATUS_OK) return 2;
    if (umi_plugin_extension_host_abi_descriptor_fingerprint(&value) == 0U) return 3;
    return 0;
}
