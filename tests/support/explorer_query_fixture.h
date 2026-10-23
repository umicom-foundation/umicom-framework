/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/support/explorer_query_fixture.h
 * PURPOSE: Inject local projection and workspace refusals while exercising production Explorer composition.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#define umi_test_explorer_session_create QueryFixture_umi_test_explorer_session_create
#define umi_test_explorer_session_create_bound QueryFixture_umi_test_explorer_session_create_bound
#define umi_test_explorer_session_destroy QueryFixture_umi_test_explorer_session_destroy
#define umi_test_explorer_session_refresh QueryFixture_umi_test_explorer_session_refresh
#define UmiTestExplorerSessionApplyFilter QueryFixture_UmiTestExplorerSessionApplyFilter
#define UmiTestExplorerSessionBindingMatches QueryFixture_UmiTestExplorerSessionBindingMatches
#define umi_test_explorer_session_set_filter QueryFixture_umi_test_explorer_session_set_filter
#define umi_test_explorer_session_set_sort QueryFixture_umi_test_explorer_session_set_sort
#define umi_test_explorer_session_set_group QueryFixture_umi_test_explorer_session_set_group
#define umi_test_explorer_session_select QueryFixture_umi_test_explorer_session_select
#define umi_test_explorer_session_build_rerun QueryFixture_umi_test_explorer_session_build_rerun
#define umi_test_explorer_session_begin QueryFixture_umi_test_explorer_session_begin
#define umi_test_explorer_session_request_stop QueryFixture_umi_test_explorer_session_request_stop
#define umi_test_explorer_session_finish QueryFixture_umi_test_explorer_session_finish
#define umi_test_explorer_session_snapshot QueryFixture_umi_test_explorer_session_snapshot
#define umi_test_explorer_session_providers QueryFixture_umi_test_explorer_session_providers
#define umi_test_explorer_session_model QueryFixture_umi_test_explorer_session_model
#define umi_test_explorer_session_rerun QueryFixture_umi_test_explorer_session_rerun
#define umi_test_explorer_session_coverage QueryFixture_umi_test_explorer_session_coverage
#define umi_test_explorer_session_benchmarks QueryFixture_umi_test_explorer_session_benchmarks
#define umi_test_explorer_session_history QueryFixture_umi_test_explorer_session_history
#define umi_test_explorer_session_state_text QueryFixture_umi_test_explorer_session_state_text
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/test_platform/explorer_model.h"
static size_t calls, live;
static size_t reject = SIZE_MAX;
static void *ExplorerTestCalloc(size_t count, size_t size)
{
    if (calls++ == reject) return NULL;
    void *value = calloc(count, size);
    if (value != NULL) ++live;
    return value;
}
static void *ExplorerTestRealloc(void *old, size_t size)
{
    if (calls++ == reject) return NULL;
    const int absent = old == NULL;
    void *value = realloc(old, size);
    if (value != NULL && absent) ++live;
    return value;
}
static void ExplorerTestFree(void *value)
{
    if (value != NULL) --live;
    free(value);
}
/* Compile the production implementation with local allocation interception.
 * Renamed public symbols avoid colliding with the linked registry library;
 * no allocation hooks or test-only failure controls enter the product ABI. */
#define umi_test_explorer_model_create ExplorerTest_umi_test_explorer_model_create
#define umi_test_explorer_model_destroy ExplorerTest_umi_test_explorer_model_destroy
#define umi_test_explorer_model_set_filter ExplorerTest_umi_test_explorer_model_set_filter
#define umi_test_explorer_model_set_sort ExplorerTest_umi_test_explorer_model_set_sort
#define umi_test_explorer_model_set_group ExplorerTest_umi_test_explorer_model_set_group
#define umi_test_explorer_model_rebuild ExplorerTest_umi_test_explorer_model_rebuild
#define umi_test_explorer_model_set_expanded ExplorerTest_umi_test_explorer_model_set_expanded
#define umi_test_explorer_model_expand_all ExplorerTest_umi_test_explorer_model_expand_all
#define umi_test_explorer_model_collapse_all ExplorerTest_umi_test_explorer_model_collapse_all
#define umi_test_explorer_model_select ExplorerTest_umi_test_explorer_model_select
#define umi_test_explorer_model_clear_selection ExplorerTest_umi_test_explorer_model_clear_selection
#define umi_test_explorer_model_selection ExplorerTest_umi_test_explorer_model_selection
#define umi_test_explorer_model_row_at ExplorerTest_umi_test_explorer_model_row_at
#define umi_test_explorer_model_visible_row_at ExplorerTest_umi_test_explorer_model_visible_row_at
#define umi_test_explorer_model_snapshot ExplorerTest_umi_test_explorer_model_snapshot
#define umi_test_explorer_model_count ExplorerTest_umi_test_explorer_model_count
#define umi_test_explorer_model_visible_count ExplorerTest_umi_test_explorer_model_visible_count
#define umi_test_explorer_model_revision ExplorerTest_umi_test_explorer_model_revision
#define UmiTestExplorerModelApplyQuery ExplorerTest_UmiTestExplorerModelApplyQuery
#define UmiExplorerPrepareQuery ExplorerTest_UmiExplorerPrepareQuery
#define UmiExplorerPublishQuery ExplorerTest_UmiExplorerPublishQuery
#define calloc ExplorerTestCalloc
#define realloc ExplorerTestRealloc
#define free ExplorerTestFree
#include "../../src/test_platform/explorer_model.c"
#undef calloc
#undef realloc
#undef free

#include "umicom/test_platform/explorer_session.h"
static UmiStatus query_workspace_refusal = UMI_STATUS_OK;
static UmiStatus QueryFixtureWorkspaceApply(UmiTestWorkspace *workspace,
    const UmiTestPlatformFilter *filter, UmiTestPlatformFilterSummary *summary)
{
    if (query_workspace_refusal != UMI_STATUS_OK) return query_workspace_refusal;
    return UmiTestWorkspaceApplyFilter(workspace, filter, summary);
}
#define UmiTestWorkspaceApplyFilter QueryFixtureWorkspaceApply
#include "../../src/test_platform/explorer_session.c"
#undef UmiTestWorkspaceApplyFilter
