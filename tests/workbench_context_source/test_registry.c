/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_source/test_registry.c
 *
 * PURPOSE:
 *   Verify live source registration, lookup, replacement and removal.
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

#include "umicom/workbench_context_source/registry.h"

int main(void)
{
    UmiWorkbenchContextSourceRegistry registry;
    UmiWorkbenchContextSourceDefinition definition;
    umi_workbench_context_source_registry_init(&registry);
    umi_workbench_context_source_definition_init(
        &definition, "source");
    assert(umi_workbench_context_source_definition_set_identity(
        &definition, "application", "panel", "Source") == UMI_STATUS_OK);
    assert(umi_workbench_context_source_registry_upsert(
        &registry, &definition) == UMI_STATUS_OK);
    assert(umi_workbench_context_source_registry_count(&registry) == 1U);
    assert(umi_workbench_context_source_registry_find(
        &registry, "source") != NULL);
    definition.minimum_interval_ms = 10U;
    assert(umi_workbench_context_source_registry_upsert(
        &registry, &definition) == UMI_STATUS_OK);
    assert(umi_workbench_context_source_registry_count(&registry) == 1U);
    assert(umi_workbench_context_source_registry_remove(
        &registry, "source") == UMI_STATUS_OK);
    umi_workbench_context_source_registry_destroy(&registry);

    return 0;
}
