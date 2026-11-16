/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/provider_connections/task_store.h
 *
 * PURPOSE:
 *   Persist scoped remote task receipts through the shared Data Server without submitting provider work.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_PROVIDER_CONNECTIONS_TASK_STORE_H
#define UMICOM_PROVIDER_CONNECTIONS_TASK_STORE_H
#include "umicom/provider_connections/tasks.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProviderTasks UmiProviderTasks;
/* These storage calls serialize transactions, not external provider effects.
 * task_owner.h composes a cooperative native guard with persisted receipt
 * checks when a host needs one effect owner across submission or recovery. */
/* Borrow the host's Data Server until Destroy. Serialize calls through the
 * store; join all users before destroying either object. No database, worker
 * thread, network connection or credentials are created here. Failed Open
 * leaves out_store unchanged. Use a persistent Data Server for restart data;
 * the memory backend intentionally lasts only as long as that server. */
UmiStatus UmiProviderTasksOpen(UmiDataServer *server,const char *application_id,
    const char *profile_id,UmiProviderTasks **out_store);
void UmiProviderTasksDestroy(UmiProviderTasks *store);
/* Save only a valid PREPARED receipt at revision one in this exact scope.
 * Existing local IDs are never overwritten, including damaged records. The
 * connection binding is supplied by the host, not reread or authorized here. */
UmiStatus UmiProviderTasksCreate(UmiProviderTasks *store,const UmiProviderTaskReceipt *prepared);
UmiStatus UmiProviderTasksRead(UmiProviderTasks *store,const char *local_id,
    UmiProviderTaskReceipt *out_receipt);
/* Load, compare revision, apply the pure transition and commit in one owned
 * transaction. Output is published only after commit and remains byte-for-byte
 * unchanged on every failure. A matching no-op still checks the revision.
 * BUSY never silently retries; reread and review. Failed begin never rolls
 * back another caller's work. Unconfirmed rollback poisons this store: join
 * workers and recover/reopen the Data Server before creating a replacement.
 * Borrowed input strings/digests may refer into out_receipt for serialized calls. */
UmiStatus UmiProviderTasksAdvance(UmiProviderTasks *store,const char *local_id,
    uint64_t expected_revision,UmiProviderTaskEvent event,UmiProviderTaskReceipt *out_receipt);
UmiStatus UmiProviderTasksConfirm(UmiProviderTasks *store,const char *local_id,
    uint64_t expected_revision,const char *remote_id,UmiProviderTaskReceipt *out_receipt);
UmiStatus UmiProviderTasksRecordOutput(UmiProviderTasks *store,const char *local_id,
    uint64_t expected_revision,uint64_t bytes,const unsigned char sha256[UMI_SHA256_BYTES],
    UmiProviderTaskReceipt *out_receipt);
/* Bounded task discovery below extends the initial storage surface; retain the earlier capability boundary for review. The former implementation is retained for owner review. */
#if 0
/* No reset/delete, automatic resubmission, enumeration or worker lease. A
 * successful uncertain-state save is a prerequisite for the host's approved
 * submission; it does not mean a request occurred. Recovery plans still need
 * current connection review, host authorization and one effect owner. External
 * database edits and restoring old files bypass this revision protocol. */

#endif
/* No reset/delete, automatic resubmission or worker lease. A
 * successful uncertain-state save is a prerequisite for the host's approved
 * submission; it does not mean a request occurred. Recovery plans still need
 * current connection review, host authorization and one effect owner. External
 * database edits and restoring old files bypass this revision protocol. */


#define UMI_PROVIDER_TASK_PAGE_CAPACITY 16U
#define UMI_PROVIDER_TASK_CATALOGUE_LIMIT 256U
#define UMI_PROVIDER_TASK_SCAN_LIMIT 32768U
typedef struct UmiProviderTaskQuery {
    UmiProviderTaskPhase phase; /* zero selects all phases */
    size_t limit; /* 1..PAGE_CAPACITY */
    char after_id[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    char expected_snapshot[UMI_SHA256_HEX_CAPACITY];
} UmiProviderTaskQuery;
typedef struct UmiProviderTaskPage {
    char application_id[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    char profile_id[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    UmiProviderTaskPhase phase;
    char snapshot[UMI_SHA256_HEX_CAPACITY];
    char next_after_id[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    size_t count,matching_count,scope_count;
    UmiProviderTaskReceipt items[UMI_PROVIDER_TASK_PAGE_CAPACITY];
} UmiProviderTaskPage;
/* Start with empty after_id and expected_snapshot. Continue with the returned
 * next_after_id and snapshot, preserving phase; changing limit is permitted.
 * Empty next_after_id means no next page. Both continuation fields are required
 * together. Changed scoped records/filter return BUSY; unknown cursor returns
 * INVALID_STATE. Refresh explicitly instead of appending a mixed-generation list.
 *
 * Examine at most SCAN_LIMIT total Data Server rows and retain at most
 * CATALOGUE_LIMIT receipts in this scope. Exceeding either refuses the whole
 * page. All selected-scope records are validated, even outside the page/filter.
 * Sorting uses local identifier bytes, independent of backend visitation order.
 * Snapshot covers all scoped canonical records, scope and phase, not credentials
 * or remote state. It is a content observation, not a signature or worker lease.
 * One owned read transaction completes before output publication. No callbacks,
 * UI or provider effects occur. Host owns worker dispatch and authorization. */
UmiStatus UmiProviderTasksReadPage(UmiProviderTasks *store,const UmiProviderTaskQuery *query,
    UmiProviderTaskPage *out_page);

#ifdef __cplusplus
}
#endif
#endif
