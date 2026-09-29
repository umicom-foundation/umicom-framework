/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/enterprise_recovery/lesson.c
 * PURPOSE: Follow a stale stock import into a new reviewed job and browse its owned result.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "lesson.h"
#include "umicom/enterprise_workspace/practice.h"
#include "umicom/enterprise_workspace/recovery.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ACT(index) UmiEnterprisePracticeActor(index)
#define CHECK(call) do { status = (call); if (status != UMI_STATUS_OK) goto cleanup; } while (0)
static UmiStatus ApplyCsv(UmiEnterpriseWorkspace *workspace, const char *jobId, const char *csv)
{
    UmiStatus status = UmiEnterpriseWorkspacePrepare(workspace, ACT(1), jobId, "supplies", "stock.csv", csv, strlen(csv), NULL);
    if (status == UMI_STATUS_OK) status = UmiEnterpriseWorkspaceReview(workspace, ACT(2), jobId, true, "Checked the stock sheet");
    if (status == UMI_STATUS_OK) status = UmiEnterpriseWorkspaceExecute(workspace, ACT(3), jobId);
    return status;
}
int UmicomEnterpriseRecoveryLesson(bool detailed)
{
    UmiDataServer *data = NULL;
    UmiEnterprisePracticeAccess *access = NULL;
    UmiEnterpriseWorkspace *workspace = NULL;
    UmiEnterpriseRecoveryReview *review = NULL;
    UmiEnterpriseDatasetView *view = NULL;
    UmiEnterpriseRowQuery query;
    UmiEnterpriseRowPage page;
    UmiEnterpriseJob job;
    UmiStatus status = UMI_STATUS_OK;
    char *report = NULL;
    const char opening[] = "item_id,label,quantity\nnotebooks,Workshop notebooks,12\npencils,Pencils,8\n";
    const char delivery[] = "item_id,label,quantity\nnotebooks,Workshop notebooks,20\n";
    const char count[] = "item_id,label,quantity\nnotebooks,Workshop notebooks,15\n";
    CHECK(umi_data_server_create_memory(&data));
    CHECK(UmiEnterprisePracticeAccessCreate(&access));
    CHECK(UmiEnterpriseWorkspaceOpen(data, UmiEnterprisePracticeAuthorisation(access), &workspace));
    CHECK(UmiEnterpriseWorkspaceSetRecipe(workspace, ACT(4), "stock.csv", true));
    CHECK(UmiEnterpriseWorkspaceCreateDataset(workspace, ACT(1), "supplies", "Workshop supplies"));
    CHECK(ApplyCsv(workspace, "opening", opening));
    CHECK(UmiEnterpriseWorkspacePrepare(workspace, ACT(1), "delivery", "supplies", "stock.csv", delivery, strlen(delivery), NULL));
    CHECK(UmiEnterpriseWorkspaceReview(workspace, ACT(2), "delivery", true, "Reviewed against twelve notebooks"));
    CHECK(ApplyCsv(workspace, "stock-count", count));
    status = UmiEnterpriseWorkspaceExecute(workspace, ACT(3), "delivery");
    if (status != UMI_STATUS_BUSY) { status = UMI_STATUS_INTERNAL_ERROR; goto cleanup; }
    puts("The approved delivery expected 12 notebooks. A stock count now records 15; the old job is refused.");
    CHECK(UmiEnterpriseRecoveryInspect(workspace, ACT(1), "delivery", &review, NULL));
    if (detailed) {
        report = malloc(32768U);
        if (report == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto cleanup; }
        CHECK(UmiEnterpriseRecoveryFormat(review, report, 32768U));
        puts(report);
    }
    CHECK(UmiEnterpriseRecoveryPrepare(workspace, ACT(1), review, "delivery-reviewed", NULL));
    CHECK(UmiEnterpriseWorkspaceJobFind(workspace, "delivery-reviewed", &job));
    if (job.state != UMI_ENTERPRISE_JOB_REVIEW || job.reviewer[0] != '\0') { status = UMI_STATUS_INTERNAL_ERROR; goto cleanup; }
    puts("A new job reviews 15 -> 20. The old job remains; its approval was not copied.");
    CHECK(UmiEnterpriseWorkspaceReview(workspace, ACT(2), "delivery-reviewed", true, "Inspected the new count before approval"));
    CHECK(UmiEnterpriseWorkspaceExecute(workspace, ACT(3), "delivery-reviewed"));
    UmiEnterpriseRowQueryInit(&query);
    (void)snprintf(query.text, sizeof(query.text), "notebooks");
    CHECK(UmiEnterpriseDatasetViewCapture(workspace, ACT(0), "supplies", &query, &view));
    /* The result owns copied rows, not borrowed database or widget pointers. */
    UmiEnterpriseWorkspaceDestroy(workspace); workspace = NULL;
    UmiEnterprisePracticeAccessDestroy(access); access = NULL;
    umi_data_server_destroy(data); data = NULL;
    CHECK(UmiEnterpriseDatasetViewPage(view, 0U, 1U, &page));
    if (page.count != 1U || page.rows[0].quantity != 20U) { status = UMI_STATUS_INTERNAL_ERROR; goto cleanup; }
    printf("Copied result: %s = %" PRIu64 " | source job %s\n", page.rows[0].id, page.rows[0].quantity, page.rows[0].sourceJob);
    puts("Practice complete. Memory only; no files, network, scheduler or financial transaction was used.");
cleanup:
    free(report);
    UmiEnterpriseDatasetViewDestroy(view);
    UmiEnterpriseRecoveryReviewDestroy(review);
    UmiEnterpriseWorkspaceDestroy(workspace);
    UmiEnterprisePracticeAccessDestroy(access);
    umi_data_server_destroy(data);
    if (status != UMI_STATUS_OK) fprintf(stderr, "Enterprise recovery lesson failed: status %d\n", (int)status);
    return status == UMI_STATUS_OK ? 0 : 1;
}
