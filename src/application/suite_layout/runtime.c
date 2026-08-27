/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/suite_layout/runtime.c
 *
 * PURPOSE:
 *   Implement application layout loading, selection and snapshots over the canonical experience catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/suite_layout/runtime.h"

#include <stdio.h>
#include <string.h>
#include "umicom/application/experience_catalogue.h"

void umi_application_suite_layout_runtime_init(
    UmiApplicationSuiteLayoutRuntime *runtime)
{
    if (runtime != NULL) {
        (void)memset(runtime, 0, sizeof(*runtime));
        runtime->revision = UINT64_C(1);
    }
}

UmiStatus umi_application_suite_layout_runtime_load(
    UmiApplicationSuiteLayoutRuntime *runtime,
    const char *application_id)
{
    const UmiApplicationExperienceDefinition *experience;
    UmiStatus status;
    if (runtime == NULL || application_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    experience = umi_application_experience_catalogue_find(application_id);
    if (experience == NULL) return UMI_STATUS_NOT_FOUND;
    status = umi_application_suite_layout_project(
        experience, experience->default_layout_id, &runtime->active_layout);
    if (status != UMI_STATUS_OK) return status;
    runtime->experience = experience;
    runtime->loaded = 1;
    runtime->revision++;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_suite_layout_runtime_select(
    UmiApplicationSuiteLayoutRuntime *runtime,
    const char *layout_id)
{
    UmiUiWorkspaceLayout projected;
    UmiStatus status;
    if (runtime == NULL || layout_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!runtime->loaded || runtime->experience == NULL)
        return UMI_STATUS_INVALID_STATE;
    status = umi_application_suite_layout_project(
        runtime->experience, layout_id, &projected);
    if (status != UMI_STATUS_OK) return status;
    runtime->active_layout = projected;
    runtime->revision++;
    return UMI_STATUS_OK;
}

const UmiUiWorkspaceLayout *umi_application_suite_layout_runtime_active(
    const UmiApplicationSuiteLayoutRuntime *runtime)
{
    return runtime != NULL && runtime->loaded ? &runtime->active_layout : NULL;
}

UmiApplicationSuiteLayoutSnapshot umi_application_suite_layout_runtime_snapshot(
    const UmiApplicationSuiteLayoutRuntime *runtime)
{
    UmiApplicationSuiteLayoutSnapshot snapshot;
    (void)memset(&snapshot, 0, sizeof(snapshot));
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
