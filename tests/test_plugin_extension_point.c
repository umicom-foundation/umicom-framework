/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_plugin_extension_point.c
 *
 * PURPOSE:
 *   Verify typed extension points and cardinality enforcement.
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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiPluginExtensionPointRegistry *points = NULL;
    UmiPluginContributionRegistry *contributions = NULL;
    UmiPluginExtensionPoint point;
    UmiPluginContribution contribution;
    char reason[256];
    (void)memset(&point, 0, sizeof(point));
    (void)strcpy(point.extension_id, "studio.sidebar");
    (void)strcpy(point.owner_id, "org.umicom.studio");
    (void)strcpy(point.schema, "pane-id");
    point.cardinality = UMI_PLUGIN_EXTENSION_ONE;
    assert(umi_plugin_extension_point_registry_create(&points) == UMI_STATUS_OK);
    assert(umi_plugin_contribution_registry_create(&contributions) == UMI_STATUS_OK);
    assert(umi_plugin_extension_point_registry_add(points, &point) == UMI_STATUS_OK);
    assert(umi_plugin_extension_point_registry_count(points) == 1U);
    (void)memset(&contribution, 0, sizeof(contribution));
    (void)strcpy(contribution.plugin_id, "org.example.one");
    (void)strcpy(contribution.contribution_type, "studio.sidebar");
    (void)strcpy(contribution.contribution_id, "example.pane");
    (void)strcpy(contribution.target, "studio.pane.example");
    assert(umi_plugin_extension_point_validate_contribution(
        points, contributions, &contribution, reason, sizeof(reason)) == UMI_STATUS_OK);
    assert(umi_plugin_contribution_registry_add(contributions, &contribution) == UMI_STATUS_OK);
    (void)strcpy(contribution.contribution_id, "example.second");
    assert(umi_plugin_extension_point_validate_contribution(
        points, contributions, &contribution, reason, sizeof(reason)) ==
        UMI_STATUS_CAPACITY_EXCEEDED);
    umi_plugin_contribution_registry_destroy(contributions);
    umi_plugin_extension_point_registry_destroy(points);
    return 0;
}
