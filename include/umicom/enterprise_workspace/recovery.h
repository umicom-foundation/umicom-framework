/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/enterprise_workspace/recovery.h
 * PURPOSE: Expose owned dataset pages and reviewed re-preparation through the existing enterprise authority.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_ENTERPRISE_WORKSPACE_RECOVERY_H
#define UMICOM_ENTERPRISE_WORKSPACE_RECOVERY_H
#include "umicom/enterprise_workspace/workspace.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_ENTERPRISE_QUERY_PAGE_ROWS 16U

typedef enum UmiEnterpriseRowSort {
    UMI_ENTERPRISE_SORT_ID = 1,
    UMI_ENTERPRISE_SORT_LABEL,
    UMI_ENTERPRISE_SORT_QUANTITY
} UmiEnterpriseRowSort;
/* Literal, case-sensitive UTF-8 substring in ID, label or source-job ID.
 * Quantity endpoints are inclusive. Sorting compares UTF-8 bytes, not a locale;
 * ties always use ascending ID, even when the primary key is descending. */
typedef struct UmiEnterpriseRowQuery {
    char text[UMI_ENTERPRISE_LABEL_CAPACITY];
    UmiEnterpriseRowSort sort;
    bool descending;
    uint64_t minimumQuantity;
    uint64_t maximumQuantity;
} UmiEnterpriseRowQuery;
typedef struct UmiEnterpriseDatasetViewInfo {
    UmiEnterpriseDataset dataset;
    uint64_t workspaceRevision;
    size_t matchedRows;
    UmiEnterpriseRowQuery query;
} UmiEnterpriseDatasetViewInfo;
typedef struct UmiEnterpriseRowPage {
    uint64_t workspaceRevision, datasetGeneration;
    size_t totalRows, offset, count, nextOffset;
    bool complete;
    UmiEnterpriseRow rows[UMI_ENTERPRISE_QUERY_PAGE_ROWS];
} UmiEnterpriseRowPage;
typedef struct UmiEnterpriseDatasetView UmiEnterpriseDatasetView;
void UmiEnterpriseRowQueryInit(UmiEnterpriseRowQuery *query);
/* Authorises enterprise.read for datasetId. Captures the loaded state without
 * reloading storage or running caller SQL. The owner must serialise capture
 * with workspace mutations. The resulting immutable view owns all its rows
 * and may outlive the workspace. Reading it later does not recheck permissions;
 * the host is responsible for access to the copy and its lifetime. */
UmiStatus UmiEnterpriseDatasetViewCapture(const UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseActor actor, const char *datasetId,
    const UmiEnterpriseRowQuery *query, UmiEnterpriseDatasetView **outView);
void UmiEnterpriseDatasetViewDestroy(UmiEnterpriseDatasetView *view);
UmiStatus UmiEnterpriseDatasetViewDescribe(const UmiEnterpriseDatasetView *view,
    UmiEnterpriseDatasetViewInfo *outInfo);
/* limit is 1..16. offset==total is a successful empty final page; an offset
 * greater than total is rejected. Failed scalar queries preserve the output. */
UmiStatus UmiEnterpriseDatasetViewPage(const UmiEnterpriseDatasetView *view,
    size_t offset, size_t limit, UmiEnterpriseRowPage *outPage);

typedef struct UmiEnterpriseRecoveryReview UmiEnterpriseRecoveryReview;
typedef struct UmiEnterpriseRecoveryInfo {
    UmiEnterpriseJob originalJob;
    UmiEnterprisePreview originalPreview;
    UmiEnterprisePreview proposedPreview;
    char preparer[UMI_ENTERPRISE_ID_CAPACITY];
    bool targetChanged;
    bool executionPaused;
} UmiEnterpriseRecoveryInfo;
/* Re-evaluate a non-applied job's frozen CSV against the loaded dataset. This
 * is not execution, approval, cancellation, or a repair of storage corruption.
 * Both enterprise.read on the old job and enterprise.job.prepare on the
 * dataset are required. Paused execution does not prevent preparation.
 * Applied jobs cannot be recovered: repeat their original job ID instead.
 * No new job or durable audit is written by this inspection. */
UmiStatus UmiEnterpriseRecoveryInspect(UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseActor actor, const char *originalJobId,
    UmiEnterpriseRecoveryReview **outReview, UmiEnterpriseIssue *outIssue);
void UmiEnterpriseRecoveryReviewDestroy(UmiEnterpriseRecoveryReview *review);
UmiStatus UmiEnterpriseRecoveryDescribe(const UmiEnterpriseRecoveryReview *review,
    UmiEnterpriseRecoveryInfo *outInfo);
/* New ID must be unused. Reject any intervening workspace change or a change
 * of preparer/role. Recheck current permissions and stored bytes. A successful
 * commit writes one new REVIEW job and job.reprepare audit (target=new ID,
 * detail=original ID) in one existing Data Server transaction. Neither the
 * original job nor its approval is changed, and no approval is copied. */
UmiStatus UmiEnterpriseRecoveryPrepare(UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseActor actor, const UmiEnterpriseRecoveryReview *review,
    const char *newJobId, UmiEnterpriseIssue *outIssue);
/* Plain text, not HTML/terminal escaping. On failure a supplied nonzero
 * buffer is emptied. Never infer approval from an inspection report. */
UmiStatus UmiEnterpriseRecoveryFormat(const UmiEnterpriseRecoveryReview *review,
    char *buffer, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
