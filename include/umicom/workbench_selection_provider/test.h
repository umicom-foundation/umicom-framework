/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/test.h
 *
 * PURPOSE:
 *   Adapt canonical Test Platform items and Test Explorer rows into structured test selections.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_TEST_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_TEST_H

#include "umicom/test_platform/explorer_model.h"
#include "umicom/workbench_selection_provider/identity.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_workbench_selection_provider_from_test_item(
    const UmiTestPlatformItemSnapshot *item,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection);

UmiStatus umi_workbench_selection_provider_from_test_row(
    const UmiTestExplorerRow *row,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection);

UmiStatus umi_workbench_selection_provider_test_model_find(
    const UmiTestExplorerModel *model,
    const char *item_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection);

UmiStatus umi_workbench_selection_provider_test_model_visible_at(
    const UmiTestExplorerModel *model,
    size_t visible_index,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection);

#ifdef __cplusplus
}
#endif
#endif
