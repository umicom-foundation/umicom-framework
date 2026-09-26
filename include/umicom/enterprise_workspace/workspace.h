/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/enterprise_workspace/workspace.h
 *
 * PURPOSE:
 *   Define reviewed, transactional reference-data imports shared by enterprise applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_ENTERPRISE_WORKSPACE_WORKSPACE_H
#define UMICOM_ENTERPRISE_WORKSPACE_WORKSPACE_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/data/data_server.h"
#include "umicom/security/authorisation.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_ENTERPRISE_ID_CAPACITY 64U
#define UMI_ENTERPRISE_LABEL_CAPACITY 192U
#define UMI_ENTERPRISE_CSV_CAPACITY 16385U
#define UMI_ENTERPRISE_MAX_DATASETS 8U
#define UMI_ENTERPRISE_MAX_ROWS 64U
#define UMI_ENTERPRISE_MAX_IMPORT_ROWS 32U
#define UMI_ENTERPRISE_MAX_JOBS 16U
#define UMI_ENTERPRISE_MAX_AUDIT 128U
#define UMI_ENTERPRISE_RECIPE_COUNT 2U

/* Identity is supplied by a trusted composition, not authenticated here.
 * The native lesson uses conspicuously labelled practice identities. */
typedef struct UmiEnterpriseActor {
    const char *principal;
    const char *role;
} UmiEnterpriseActor;

typedef struct UmiEnterpriseRow {
    char id[UMI_ENTERPRISE_ID_CAPACITY];
    char label[UMI_ENTERPRISE_LABEL_CAPACITY];
    uint64_t quantity;
    char sourceJob[UMI_ENTERPRISE_ID_CAPACITY];
} UmiEnterpriseRow;

typedef struct UmiEnterpriseDataset {
    char id[UMI_ENTERPRISE_ID_CAPACITY];
    char name[UMI_ENTERPRISE_LABEL_CAPACITY];
    uint64_t generation;
    size_t rowCount;
} UmiEnterpriseDataset;

typedef enum UmiEnterpriseChangeKind {
    UMI_ENTERPRISE_INSERT = 1,
    UMI_ENTERPRISE_UPDATE = 2,
    UMI_ENTERPRISE_UNCHANGED = 3
} UmiEnterpriseChangeKind;

typedef struct UmiEnterpriseChange {
    UmiEnterpriseChangeKind kind;
    UmiEnterpriseRow before;
    UmiEnterpriseRow after;
} UmiEnterpriseChange;

typedef struct UmiEnterprisePreview {
    char datasetId[UMI_ENTERPRISE_ID_CAPACITY];
    char recipeId[UMI_ENTERPRISE_ID_CAPACITY];
    uint64_t datasetGeneration;
    uint64_t workspaceRevision;
    size_t rowCount, insertCount, updateCount, unchangedCount;
    UmiEnterpriseChange changes[UMI_ENTERPRISE_MAX_IMPORT_ROWS];
} UmiEnterprisePreview;

typedef struct UmiEnterpriseIssue {
    size_t record; /* Logical CSV record, header = 1; zero for non-CSV errors. */
    size_t column; /* One-based field; zero when not applicable. */
    char message[256];
} UmiEnterpriseIssue;

typedef enum UmiEnterpriseJobState {
    UMI_ENTERPRISE_JOB_REVIEW = 1,
    UMI_ENTERPRISE_JOB_APPROVED = 2,
    UMI_ENTERPRISE_JOB_APPLIED = 3,
    UMI_ENTERPRISE_JOB_REJECTED = 4,
    UMI_ENTERPRISE_JOB_CANCELLED = 5
} UmiEnterpriseJobState;

typedef struct UmiEnterpriseJob {
    char id[UMI_ENTERPRISE_ID_CAPACITY];
    char datasetId[UMI_ENTERPRISE_ID_CAPACITY];
    char recipeId[UMI_ENTERPRISE_ID_CAPACITY];
    char author[UMI_ENTERPRISE_ID_CAPACITY];
    /* Current decision actor. For cancellation this is the cancelling actor;
     * an earlier approval remains in the chronological audit entries. */
    char reviewer[UMI_ENTERPRISE_ID_CAPACITY];
    char executor[UMI_ENTERPRISE_ID_CAPACITY];
    char reason[UMI_ENTERPRISE_LABEL_CAPACITY];
    UmiEnterpriseJobState state;
    uint64_t datasetGeneration, preparedRevision, decisionRevision, appliedRevision;
    size_t rowCount, insertCount, updateCount, unchangedCount;
} UmiEnterpriseJob;

typedef struct UmiEnterpriseRecipe {
    char id[UMI_ENTERPRISE_ID_CAPACITY];
    char name[UMI_ENTERPRISE_LABEL_CAPACITY];
    char description[256];
    unsigned contractVersion;
    bool enabled;
} UmiEnterpriseRecipe;

typedef struct UmiEnterpriseAuditEntry {
    uint64_t revision;
    char principal[UMI_ENTERPRISE_ID_CAPACITY];
    char action[UMI_ENTERPRISE_ID_CAPACITY];
    char target[UMI_ENTERPRISE_ID_CAPACITY];
    char detail[UMI_ENTERPRISE_LABEL_CAPACITY];
} UmiEnterpriseAuditEntry;

typedef struct UmiEnterpriseSnapshot {
    uint64_t revision;
    size_t datasetCount, rowCount, jobCount, pendingCount, appliedCount, auditCount;
    bool paused;
    bool storageFault; /* A failed rollback requires closing this instance. */
} UmiEnterpriseSnapshot;

typedef struct UmiEnterpriseWorkspace UmiEnterpriseWorkspace;

/* One owner thread. Data Server and authorisation are borrowed and must outlive
 * this workspace. Do not share an active transaction or mutate either service
 * concurrently. Open and Reload only read; neither executes an import. Missing
 * storage starts empty with both recipes disabled. Other namespaces are left
 * untouched. A malformed saved workspace is never replaced with empty state. */
UmiStatus UmiEnterpriseWorkspaceOpen(UmiDataServer *data,
    UmiAuthorisationService *authorisation, UmiEnterpriseWorkspace **outWorkspace);
void UmiEnterpriseWorkspaceDestroy(UmiEnterpriseWorkspace *workspace);
UmiStatus UmiEnterpriseWorkspaceReload(UmiEnterpriseWorkspace *workspace);
UmiStatus UmiEnterpriseWorkspaceSnapshot(const UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseSnapshot *outSnapshot);

/* Queries copy the caller-authorised local workspace. These are internal C
 * contracts, not network endpoints: a host must authorise its read access. */
UmiStatus UmiEnterpriseWorkspaceDatasetAt(const UmiEnterpriseWorkspace *workspace,
    size_t index, UmiEnterpriseDataset *outDataset);
UmiStatus UmiEnterpriseWorkspaceRowAt(const UmiEnterpriseWorkspace *workspace,
    const char *datasetId, size_t index, UmiEnterpriseRow *outRow);
UmiStatus UmiEnterpriseWorkspaceJobAt(const UmiEnterpriseWorkspace *workspace,
    size_t index, UmiEnterpriseJob *outJob);
UmiStatus UmiEnterpriseWorkspaceJobFind(const UmiEnterpriseWorkspace *workspace,
    const char *jobId, UmiEnterpriseJob *outJob);
UmiStatus UmiEnterpriseWorkspaceJobPreview(const UmiEnterpriseWorkspace *workspace,
    const char *jobId, UmiEnterprisePreview *outPreview);
UmiStatus UmiEnterpriseWorkspaceAuditAt(const UmiEnterpriseWorkspace *workspace,
    size_t index, UmiEnterpriseAuditEntry *outEntry);
UmiStatus UmiEnterpriseWorkspaceRecipeAt(const UmiEnterpriseWorkspace *workspace,
    size_t index, UmiEnterpriseRecipe *outRecipe);

/* Upserts target only this module's reference-data records. No caller-supplied SQL, network,
 * dynamic plug-in, script, financial transaction or OS action is executed. */
UmiStatus UmiEnterpriseWorkspaceCreateDataset(UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseActor actor, const char *datasetId, const char *name);
UmiStatus UmiEnterpriseWorkspaceSetRecipe(UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseActor actor, const char *recipeId, bool enabled);
UmiStatus UmiEnterpriseWorkspaceSetPaused(UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseActor actor, bool paused);
UmiStatus UmiEnterpriseWorkspaceCheckAccess(const UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseActor actor, const char *capability, const char *resource,
    UmiAuthorisationDecision *outDecision);

/* UTF-8 CSV, exact header item_id,label,quantity, LF or CRLF. Quotes, doubled
 * quotes and embedded newlines are supported. Quantity is an unsigned decimal
 * integer in [0, INT64_MAX]. No leading sign or spaces. IDs are ASCII tokens.
 * Preview is read-only. A job ID is also its idempotency key: an exact repeated
 * Prepare returns the original job; changed input under that ID is rejected. */
UmiStatus UmiEnterpriseWorkspacePreview(const UmiEnterpriseWorkspace *workspace,
    const char *datasetId, const char *recipeId, const char *csv, size_t length,
    UmiEnterprisePreview *outPreview, UmiEnterpriseIssue *outIssue);
UmiStatus UmiEnterpriseWorkspacePrepare(UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseActor actor, const char *jobId, const char *datasetId,
    const char *recipeId, const char *csv, size_t length, UmiEnterpriseIssue *outIssue);
UmiStatus UmiEnterpriseWorkspaceReview(UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseActor actor, const char *jobId, bool approve, const char *reason);
UmiStatus UmiEnterpriseWorkspaceCancel(UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseActor actor, const char *jobId, const char *reason);
/* Execute commits all rows, job outcome and audit in one transaction. BUSY
 * means stale workspace/target data: reload, inspect, and prepare a NEW job
 * where target data changed. Retrying an applied job never posts again.
 * Failure leaves the approved job retryable when rollback succeeded. */
UmiStatus UmiEnterpriseWorkspaceExecute(UmiEnterpriseWorkspace *workspace,
    UmiEnterpriseActor actor, const char *jobId);
const char *UmiEnterpriseJobStateText(UmiEnterpriseJobState state);

#ifdef __cplusplus
}
#endif
#endif
