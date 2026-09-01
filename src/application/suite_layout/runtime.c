/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/suite_layout/runtime.c
 *
 * PURPOSE:
 *   Implement application layout loading, selection and snapshots over the canonical experience catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/suite_layout/runtime.h"

#include <stdio.h>
#include <string.h>
#include "umicom/application/experience_catalogue.h"

/* Reset caller-owned runtime storage to one predictable unloaded state. */
void umi_application_suite_layout_runtime_init(
    UmiApplicationSuiteLayoutRuntime *runtime)
{
    /* A null pointer is harmless because initialisation has no result to return. */
    if (runtime != NULL) {
        (void)memset(runtime, 0, sizeof(*runtime));
        runtime->revision = UINT64_C(1);
    }
}

/* Resolve an application and make its default layout active atomically. */
UmiStatus umi_application_suite_layout_runtime_load(
    UmiApplicationSuiteLayoutRuntime *runtime,
    const char *application_id)
{
    const UmiApplicationExperienceDefinition *experience;
    UmiStatus status;
    /* Runtime storage and the stable catalogue key are both required. */
    if (runtime == NULL || application_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    experience = umi_application_experience_catalogue_find(application_id);
    /* Unknown applications cannot supply a default layout. */
    if (experience == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_application_suite_layout_project(
        experience, experience->default_layout_id, &runtime->active_layout);
    /* Publish experience and loaded state only after projection succeeds. */
    if (status != UMI_STATUS_OK) return status;
    runtime->experience = experience;
    runtime->loaded = 1;
    runtime->revision++;
    return UMI_STATUS_OK;
}

/* Project a selected canonical layout before replacing the active value. */
UmiStatus umi_application_suite_layout_runtime_select(
    UmiApplicationSuiteLayoutRuntime *runtime,
    const char *layout_id)
{
    UmiUiWorkspaceLayout projected;
    UmiStatus status;
    /* Selection needs a live runtime and stable layout key. */
    if (runtime == NULL || layout_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Loading establishes the owning experience required for selection. */
    if (!runtime->loaded || runtime->experience == NULL)
        return UMI_STATUS_INVALID_STATE;
    status = umi_application_suite_layout_project(
        runtime->experience, layout_id, &projected);
    /* Failed projection leaves the previously active layout untouched. */
    if (status != UMI_STATUS_OK) return status;
    runtime->active_layout = projected;
    runtime->revision++;
    return UMI_STATUS_OK;
}

/* Return a borrowed active layout only after loading has completed. */
const UmiUiWorkspaceLayout *umi_application_suite_layout_runtime_active(
    const UmiApplicationSuiteLayoutRuntime *runtime)
{
    return runtime != NULL && runtime->loaded ? &runtime->active_layout : NULL;
}

/* Copy runtime facts into a small value suitable for UI and diagnostics. */
UmiApplicationSuiteLayoutSnapshot umi_application_suite_layout_runtime_snapshot(
    const UmiApplicationSuiteLayoutRuntime *runtime)
{
    UmiApplicationSuiteLayoutSnapshot snapshot;
    (void)memset(&snapshot, 0, sizeof(snapshot));
    /* Invalid or unloaded input returns the safe zeroed snapshot. */
    if (runtime == NULL || !runtime->loaded || runtime->experience == NULL)
        return snapshot;
    (void)snprintf(snapshot.application_id, sizeof(snapshot.application_id), "%s",
                   runtime->experience->application_id);
    (void)snprintf(snapshot.active_layout_id, sizeof(snapshot.active_layout_id), "%s",
                   runtime->active_layout.layout_id);
    (void)snprintf(snapshot.active_layout_name, sizeof(snapshot.active_layout_name), "%s",
                   runtime->active_layout.name);
    snapshot.window_count = runtime->active_layout.window_count;
    snapshot.revision = runtime->revision + runtime->active_layout.revision;
    snapshot.loaded = 1;
    return snapshot;
}
