/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/project.h
 *
 * PURPOSE:
 *   Adapt authoritative project workspace selections and descriptors into structured workbench selections.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_PROJECT_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_PROJECT_H

#include "umicom/project/project.h"
#include "umicom/workbench_selection_provider/identity.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_workbench_selection_provider_from_project_selection(
    const UmiProjectWorkspaceSelectionSnapshot *snapshot,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection);

UmiStatus umi_workbench_selection_provider_from_project_descriptor(
    const UmiProjectDescriptorSnapshot *project,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection);

#ifdef __cplusplus
}
#endif
#endif
