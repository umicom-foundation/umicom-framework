/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_plugin_registry.c
 *
 * PURPOSE:
 *   Verify plug-in catalogue registration, state transitions and enablement.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include "umicom/plugin/registry.h"
int main(void) {
    UmiPluginRegistry *r = NULL; UmiPluginManifest m = {0}; UmiPluginRecord out;
    (void)strcpy(m.plugin_id,"org.umicom.example"); (void)strcpy(m.display_name,"Example"); (void)strcpy(m.library_path,"example.so"); m.required_abi=2U;
    assert(umi_plugin_registry_create(&r) == UMI_STATUS_OK);
    assert(umi_plugin_registry_add(r,&m,"example.umicom-plugin") == UMI_STATUS_OK);
    assert(umi_plugin_registry_transition(r,m.plugin_id,UMI_PLUGIN_VALIDATED) == UMI_STATUS_OK);
    assert(umi_plugin_registry_get(r,m.plugin_id,&out) == UMI_STATUS_OK && out.state == UMI_PLUGIN_VALIDATED);
    umi_plugin_registry_destroy(r); return 0;
}
