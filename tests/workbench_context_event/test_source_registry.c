/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_event/test_source_registry.c
 *
 * PURPOSE:
 *   Verify source registration and lookup.
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

#include "umicom/workbench_context_event/source_registry.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchContextEventSourceRegistry registry;
    UmiWorkbenchContextEventSourceDescriptor source;
    umi_workbench_context_event_source_registry_init(&registry);
    umi_workbench_context_event_source_descriptor_init(
        &source, "studio.editor");
    assert(umi_workbench_context_event_source_descriptor_set_source(
        &source, "studio.editor") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_source_registry_upsert(
        &registry, &source) == UMI_STATUS_OK);
    assert(umi_workbench_context_event_source_registry_find(
        &registry, "studio.editor") != NULL);
    assert(umi_workbench_context_event_source_registry_remove(
        &registry, "studio.editor") == UMI_STATUS_OK);
    umi_workbench_context_event_source_registry_destroy(&registry);

    return 0;
}
