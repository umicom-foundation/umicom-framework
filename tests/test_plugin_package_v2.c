/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_plugin_package_v2.c
 *
 * PURPOSE:
 *   Verify safe package inventories and recoverable management plans.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "plugin_v2_fixture.h"

int main(void)
{
    UmiPluginManifest manifest = plugin_test_manifest("org.example", "Example");
    UmiPluginPackage package;
    UmiPluginPackagePlan plan;
    char reason[256];
    assert(umi_plugin_package_init(&package, &manifest, "example.umi-extension") == UMI_STATUS_OK);
    assert(umi_plugin_package_add_file(&package, "manifest.umicom-plugin") == UMI_STATUS_OK);
    assert(umi_plugin_package_add_file(&package, "bin/example.dll") == UMI_STATUS_OK);
    assert(umi_plugin_package_add_file(&package, "../escape.dll") == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_plugin_package_validate(&package, reason, sizeof(reason)) == UMI_STATUS_OK);
    assert(umi_plugin_package_plan_create(UMI_PLUGIN_PACKAGE_UPDATE, &package,
        plugin_test_version(1U, 0U, 0U), "extensions", &plan) == UMI_STATUS_OK);
    assert(plan.step_count >= 8U);
    assert(plan.reversible == 1);
    assert(strcmp(plan.plugin_id, "org.example") == 0);
    return 0;
}
