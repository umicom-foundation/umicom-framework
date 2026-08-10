/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_plugin_host.c
 *
 * PURPOSE:
 *   Verify host-level permission grants protect plug-in registration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include "umicom/plugin/host.h"
int main(void) {
    UmiPluginHost *h = NULL; UmiPluginManifest m = {0}; UmiPluginPermissionDecision d;
    (void)strcpy(m.plugin_id,"org.umicom.example"); (void)strcpy(m.display_name,"Example"); (void)strcpy(m.library_path,"example.so"); (void)strcpy(m.permissions[0],"workspace.read"); m.permission_count=1U; m.required_abi=2U;
    assert(umi_plugin_host_create(&h) == UMI_STATUS_OK);
    assert(umi_plugin_host_register_manifest(h,&m,"example.umicom-plugin",&d) == UMI_STATUS_PERMISSION_DENIED);
    assert(umi_permission_set_add(umi_plugin_host_grants(h),"workspace.*") == UMI_STATUS_OK);
    assert(umi_plugin_host_register_manifest(h,&m,"example.umicom-plugin",&d) == UMI_STATUS_OK);
    umi_plugin_host_destroy(h); return 0;
}
