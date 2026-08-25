/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_package_identity.c
 *
 * PURPOSE:
 *   Exercise describe one extension package identity, version and publisher tuple.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/package_identity.h"
int main(void)
{
    UmiPluginExtensionHostPackageIdentity value; umi_plugin_extension_host_package_identity_init(&value);
    if (umi_plugin_extension_host_package_identity_configure(&value, "sample.extension", "evidence", 2U, 12U, UINT64_C(3)) != UMI_STATUS_OK) return 1;
    if (umi_plugin_extension_host_package_identity_validate(&value) != UMI_STATUS_OK) return 2;
    if (umi_plugin_extension_host_package_identity_fingerprint(&value) == 0U) return 3;
    return 0;
}
