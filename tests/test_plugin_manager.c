/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_plugin_manager.c
 *
 * PURPOSE:
 *   Verify manager composition, policy, enablement, contributions and audit.
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
#include "plugin_fixture.h"

int main(void)
{
    UmiPluginManager *manager = NULL;
    UmiPluginManifest manifest = plugin_test_manifest("org.example", "Example");
    UmiPluginPolicyDecision decision;
    UmiPluginManagerSnapshot snapshot;
    UmiPluginExtensionPoint point;
    UmiPluginContribution contribution;
    UmiPluginCatalogueEntry catalogue_entry;
    assert(umi_plugin_manager_create(NULL, NULL, &manager) == UMI_STATUS_OK);
    assert(umi_permission_set_add(umi_plugin_host_grants(
        umi_plugin_manager_host(manager)), "*") == UMI_STATUS_OK);
    assert(umi_plugin_manager_register(manager, &manifest,
        "example.umicom-plugin", UMI_PLUGIN_TRUST_LOCAL, 100U, &decision) == UMI_STATUS_OK);
    (void)memset(&point, 0, sizeof(point));
    (void)strcpy(point.extension_id, "studio.command");
    (void)strcpy(point.owner_id, "org.umicom.studio");
    (void)strcpy(point.schema, "command-id");
    point.cardinality = UMI_PLUGIN_EXTENSION_MANY;
    assert(umi_plugin_extension_point_registry_add(
        umi_plugin_manager_extension_points(manager), &point) == UMI_STATUS_OK);
    (void)memset(&contribution, 0, sizeof(contribution));
    (void)strcpy(contribution.plugin_id, "org.example");
    (void)strcpy(contribution.contribution_type, "studio.command");
    (void)strcpy(contribution.contribution_id, "example.hello");
    (void)strcpy(contribution.target, "studio.action.example.hello");
    assert(umi_plugin_manager_add_contribution(manager, &contribution) == UMI_STATUS_OK);
    (void)memset(&catalogue_entry, 0, sizeof(catalogue_entry));
    (void)strcpy(catalogue_entry.plugin_id, "org.example");
    (void)strcpy(catalogue_entry.display_name, "Example");
    (void)strcpy(catalogue_entry.publisher, "Umicom Foundation");
    catalogue_entry.version = (UmiVersion){2U, 0U, 0U};
    catalogue_entry.required_abi = UMICOM_FRAMEWORK_ABI_VERSION;
    catalogue_entry.verified = 1;
    assert(umi_plugin_catalogue_add(
        umi_plugin_manager_catalogue(manager), &catalogue_entry) == UMI_STATUS_OK);
    assert(umi_plugin_manager_set_enabled(manager, "org.example", 0, 200U) == UMI_STATUS_OK);
    assert(umi_plugin_manager_snapshot(manager, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.installed == 1U);
    assert(snapshot.enabled == 0U);
    assert(snapshot.contributions == 1U);
    assert(snapshot.pending_updates == 1U);
    assert(snapshot.audit_events == 2U);
    umi_plugin_manager_destroy(manager);
    return 0;
}
