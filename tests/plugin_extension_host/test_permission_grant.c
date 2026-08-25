/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/plugin_extension_host/test_permission_grant.c
 *
 * PURPOSE:
 *   Exercise describe one persisted permission grant and its decision provenance.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/plugin/extension_host/permission_grant.h"
int main(void)
{
    UmiPluginExtensionHostPermissionGrant value; umi_plugin_extension_host_permission_grant_init(&value);
    if (umi_plugin_extension_host_permission_grant_configure(&value, "sample.extension", "evidence", 2U, 12U, UINT64_C(3)) != UMI_STATUS_OK) return 1;
    if (umi_plugin_extension_host_permission_grant_validate(&value) != UMI_STATUS_OK) return 2;
    if (umi_plugin_extension_host_permission_grant_fingerprint(&value) == 0U) return 3;
    return 0;
}
