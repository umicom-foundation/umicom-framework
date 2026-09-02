/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection/test_resolver.c
 *
 * PURPOSE:
 *   Verify structured selections become source samples without parsing display text.
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

#include "umicom/workbench_selection/builders.h"
#include "umicom/workbench_selection/resolver.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchSelection selection;
    UmiWorkbenchContextSourceSample sample;

    assert(umi_workbench_selection_build_source_control_change(
        &selection,
        "vcs-selection",
        "studio",
        "studio.source-control",
        "workspace",
        "project",
        "C:/dev/project",
        "repo",
        "main",
        "src/main.c",
        "modified",
        100U) == UMI_STATUS_OK);

    assert(umi_workbench_selection_resolve_source_sample(
        &selection,
        "studio.source-control.selection",
        UMI_WORKBENCH_CONTEXT_SOURCE_SOURCE_CONTROL,
        UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_SELECT,
        &sample) == UMI_STATUS_OK);

    assert(strcmp(sample.subject_id, "project") == 0);
    assert(strcmp(sample.path, "C:/dev/project") == 0);
    assert(umi_workbench_context_source_sample_find_metadata(
        &sample, "selected-path") != NULL);
    return 0;
}
