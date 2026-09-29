/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/enterprise_workspace/query.c
 * PURPOSE: Capture stable, authorised and bounded dataset pages without a second data store.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/enterprise_workspace/recovery.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

struct UmiEnterpriseDatasetView {
    UmiEnterpriseDatasetViewInfo info;
    UmiEnterpriseRow rows[UMI_ENTERPRISE_MAX_ROWS];
};
void UmiEnterpriseRowQueryInit(UmiEnterpriseRowQuery *query)
{
    if (query == NULL) return;
    memset(query, 0, sizeof(*query));
    query->sort = UMI_ENTERPRISE_SORT_ID;
    query->maximumQuantity = (uint64_t)INT64_MAX;
}
static int Compare(const UmiEnterpriseRow *left, const UmiEnterpriseRow *right,
    const UmiEnterpriseRowQuery *query)
{
    int order;
    if (query->sort == UMI_ENTERPRISE_SORT_QUANTITY)
        order = (left->quantity > right->quantity) - (left->quantity < right->quantity);
    else {
        int raw = strcmp(query->sort == UMI_ENTERPRISE_SORT_LABEL ? left->label : left->id,
            query->sort == UMI_ENTERPRISE_SORT_LABEL ? right->label : right->id);
        order = (raw > 0) - (raw < 0); /* Never negate a possibly INT_MIN strcmp. */
    }
    if (order != 0) return query->descending ? -order : order;
    return strcmp(left->id, right->id);
}
UmiStatus UmiEnterpriseDatasetViewCapture(const UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseActor actor, const char *datasetId,
    const UmiEnterpriseRowQuery *query, UmiEnterpriseDatasetView **outView)
{
    UmiAuthorisationDecision decision;
    UmiEnterpriseDatasetView *view;
    UmiStatus status;
    size_t index;
    if (outView == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outView = NULL;
    if (workspace == NULL || query == NULL || !EwsId(datasetId) ||
        !EwsText(query->text, sizeof(query->text), true, true) ||
        query->sort < UMI_ENTERPRISE_SORT_ID || query->sort > UMI_ENTERPRISE_SORT_QUANTITY ||
        query->minimumQuantity > query->maximumQuantity ||
        query->maximumQuantity > (uint64_t)INT64_MAX) return UMI_STATUS_INVALID_ARGUMENT;
    if (workspace->storageFault) return UMI_STATUS_INVALID_STATE;
    status = UmiEnterpriseWorkspaceCheckAccess(workspace, actor, "enterprise.read", datasetId, &decision);
    if (status != UMI_STATUS_OK) return status;
    if (!decision.allowed) return UMI_STATUS_PERMISSION_DENIED;
    index = EwsDatasetIndex(workspace->state, datasetId);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    const EwsDataset *dataset = &workspace->state->datasets[index];
    if (dataset->info.rowCount > UMI_ENTERPRISE_MAX_ROWS) return UMI_STATUS_INVALID_STATE;
    view = calloc(1U, sizeof(*view));
    if (view == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    view->info.dataset = dataset->info;
    view->info.workspaceRevision = workspace->state->revision;
    view->info.query = *query;
    for (size_t i = 0U; i < dataset->info.rowCount; ++i) {
        const UmiEnterpriseRow *row = &dataset->rows[i];
        if (row->quantity < query->minimumQuantity || row->quantity > query->maximumQuantity) continue;
        if (query->text[0] != '\0' && strstr(row->id, query->text) == NULL &&
            strstr(row->label, query->text) == NULL && strstr(row->sourceJob, query->text) == NULL) continue;
        /* Stable insertion sort is bounded by the existing 64-row catalogue.
         * No global comparator context, locale or mutable view registry. */
        size_t at = view->info.matchedRows;
        while (at > 0U && Compare(row, &view->rows[at - 1U], query) < 0) {
            view->rows[at] = view->rows[at - 1U]; --at;
        }
        view->rows[at] = *row; ++view->info.matchedRows;
    }
    *outView = view;
    return UMI_STATUS_OK;
}
void UmiEnterpriseDatasetViewDestroy(UmiEnterpriseDatasetView *view) { free(view); }
UmiStatus UmiEnterpriseDatasetViewDescribe(const UmiEnterpriseDatasetView *view,
    UmiEnterpriseDatasetViewInfo *outInfo)
{
    if (view == NULL || outInfo == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outInfo = view->info; return UMI_STATUS_OK;
}
UmiStatus UmiEnterpriseDatasetViewPage(const UmiEnterpriseDatasetView *view,
    size_t offset, size_t limit, UmiEnterpriseRowPage *outPage)
{
    UmiEnterpriseRowPage page = {0};
    if (view == NULL || outPage == NULL || limit == 0U || limit > UMI_ENTERPRISE_QUERY_PAGE_ROWS)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (offset > view->info.matchedRows) return UMI_STATUS_NOT_FOUND;
    page.workspaceRevision = view->info.workspaceRevision;
    page.datasetGeneration = view->info.dataset.generation;
    page.totalRows = view->info.matchedRows; page.offset = offset;
    page.count = page.totalRows - offset;
    if (page.count > limit) page.count = limit;
    page.nextOffset = offset + page.count; page.complete = page.nextOffset == page.totalRows;
    for (size_t i = 0U; i < page.count; ++i) page.rows[i] = view->rows[offset + i];
    *outPage = page; return UMI_STATUS_OK;
}
