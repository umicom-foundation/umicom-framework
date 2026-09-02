/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/workbench/compare_model.h
 *
 * PURPOSE:
 *   Project the canonical text-diff engine and advanced comparison records into
 *   one synchronized side-by-side or inline workbench model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_WORKBENCH_COMPARE_MODEL_H
#define UMICOM_VCS_WORKBENCH_COMPARE_MODEL_H

#include "umicom/developer_productivity/diff.h"
#include "umicom/developer_productivity/diff_hunk.h"
#include "umicom/vcs/advanced/compare_navigation.h"
#include "umicom/vcs/advanced/compare_session.h"
#include "umicom/vcs/advanced/diff_document.h"
#include "umicom/vcs/advanced/diff_options.h"
#include "umicom/vcs/advanced/diff_summary.h"
#include "umicom/vcs/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs workbench compare model data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsWorkbenchCompareModel {
    uint32_t struct_size;
    uint32_t api_version;
    UmiVcsAdvancedCompareSession session;
    UmiVcsAdvancedDiffDocument document;
    UmiVcsAdvancedDiffOptions options;
    UmiVcsAdvancedDiffSummary summary;
    UmiVcsAdvancedCompareNavigation navigation;
    UmiDeveloperDiffStats stats;
    UmiDeveloperDiffRow rows[UMI_DEVELOPER_DIFF_MAX_ROWS];
    UmiDeveloperDiffHunk hunks[UMI_DEVELOPER_DIFF_MAX_HUNKS];
    size_t row_count;
    size_t hunk_count;
    size_t current_row;
    UmiVcsWorkbenchCompareViewMode view_mode;
    int ready;
    int truncated;
    uint64_t revision;
} UmiVcsWorkbenchCompareModel;

/**
 * Initialise vcs workbench compare model from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_workbench_compare_model_init(
    UmiVcsWorkbenchCompareModel *model);
/**
 * Provide the vcs workbench compare model open operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_workbench_compare_model_open(
    UmiVcsWorkbenchCompareModel *model,
    const char *session_id,
    const UmiVcsAdvancedCompareSide *left,
    const UmiVcsAdvancedCompareSide *right,
    const char *left_text,
    const char *right_text,
    const UmiVcsAdvancedDiffOptions *options);
/**
 * Provide the vcs workbench compare model set view mode operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_workbench_compare_model_set_view_mode(
    UmiVcsWorkbenchCompareModel *model,
    UmiVcsWorkbenchCompareViewMode view_mode);
/**
 * Provide the vcs workbench compare model select hunk operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_workbench_compare_model_select_hunk(
    UmiVcsWorkbenchCompareModel *model,
    size_t index);
/**
 * Provide the vcs workbench compare model next hunk operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_workbench_compare_model_next_hunk(
    UmiVcsWorkbenchCompareModel *model);
/**
 * Provide the vcs workbench compare model previous hunk operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_workbench_compare_model_previous_hunk(
    UmiVcsWorkbenchCompareModel *model);
/**
 * Find vcs workbench compare model row while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDeveloperDiffRow *umi_vcs_workbench_compare_model_row_at(
    const UmiVcsWorkbenchCompareModel *model,
    size_t index);
/**
 * Find vcs workbench compare model hunk while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiDeveloperDiffHunk *umi_vcs_workbench_compare_model_hunk_at(
    const UmiVcsWorkbenchCompareModel *model,
    size_t index);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_VCS_WORKBENCH_COMPARE_MODEL_H */
