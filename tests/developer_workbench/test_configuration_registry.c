/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_configuration_registry.c
 *
 * PURPOSE:
 *   Verify named configuration registration and activation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/developer_workbench/configuration_registry.h"

static void make_configuration(UmiDeveloperWorkbenchConfiguration *value,
                               const char *id,
                               const char *title)
{
    umi_developer_workbench_configuration_init(value, id, title);
    (void)strcpy(value->source_root, "C:/work/project");
    (void)strcpy(value->build_directory, "build/debug");
}

int main(void)
{
    UmiDeveloperWorkbenchConfigurationRegistry *registry = NULL;
    UmiDeveloperWorkbenchConfiguration debug;
    UmiDeveloperWorkbenchConfiguration release;
    UmiDeveloperWorkbenchConfiguration active;

    assert(umi_developer_workbench_configuration_registry_create(&registry) ==
           UMI_STATUS_OK);

    make_configuration(&debug, "debug", "Debug");
    make_configuration(&release, "release", "Release");

    assert(umi_developer_workbench_configuration_registry_upsert(
        registry, &debug) == UMI_STATUS_OK);
    assert(umi_developer_workbench_configuration_registry_upsert(
        registry, &release) == UMI_STATUS_OK);
    assert(umi_developer_workbench_configuration_registry_count(registry) ==
           2U);

    assert(umi_developer_workbench_configuration_registry_activate(
        registry, "release") == UMI_STATUS_OK);
    assert(umi_developer_workbench_configuration_registry_active(
        registry, &active) == UMI_STATUS_OK);
    assert(strcmp(active.configuration_id, "release") == 0);

    umi_developer_workbench_configuration_registry_destroy(registry);
    return 0;
}
