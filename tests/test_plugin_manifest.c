/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_plugin_manifest.c
 *
 * PURPOSE:
 *   Verify plug-in manifest parsing and validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/plugin/manifest.h"
int main(void) {
    const char *text = "id=org.umicom.example\nname=Example\nversion=1.0.0\nabi=2\nlibrary=example.so\npermission=workspace.read\ncapability=example.run\n";
    UmiPluginManifest m; char reason[128];
    assert(umi_plugin_manifest_parse(text,&m) == UMI_STATUS_OK);
    assert(strcmp(m.plugin_id,"org.umicom.example") == 0 && m.permission_count == 1U);
    assert(umi_plugin_manifest_validate(&m,reason,sizeof(reason)) == UMI_STATUS_OK);
    return 0;
}
