/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_plugin_contributions.c
 *
 * PURPOSE:
 *   Verify typed plug-in contribution registration and type counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include "umicom/plugin/contribution.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPluginContributionRegistry *r = NULL; UmiPluginContribution c = {0};
    (void)strcpy(c.plugin_id,"org.umicom.example"); (void)strcpy(c.contribution_type,"command"); (void)strcpy(c.contribution_id,"example.run");
    assert(umi_plugin_contribution_registry_create(&r) == UMI_STATUS_OK);
    assert(umi_plugin_contribution_registry_add(r,&c) == UMI_STATUS_OK);
    assert(umi_plugin_contribution_registry_count_type(r,"command") == 1U);
    umi_plugin_contribution_registry_destroy(r); return 0;
}
