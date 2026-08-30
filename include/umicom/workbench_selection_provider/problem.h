/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/problem.h
 *
 * PURPOSE:
 *   Adapt Framework problem registry records into structured diagnostic/source-location selections.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_PROBLEM_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_PROBLEM_H

#include "umicom/ui/problem.h"
#include "umicom/workbench_selection_provider/identity.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_workbench_selection_provider_from_problem(
    const UmiUiProblemSnapshot *problem,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection);

UmiStatus umi_workbench_selection_provider_problem_at(
    const UmiUiProblemRegistry *registry,
    size_t index,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection);

UmiStatus umi_workbench_selection_provider_problem_find(
    const UmiUiProblemRegistry *registry,
    const char *problem_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection);

#ifdef __cplusplus
}
#endif
#endif
