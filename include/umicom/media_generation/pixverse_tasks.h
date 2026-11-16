/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/pixverse_tasks.h
 *
 * PURPOSE:
 *   Bind native video requests and confirmed results to the shared provider task store.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_MEDIA_GENERATION_PIXVERSE_TASKS_H
#define UMICOM_MEDIA_GENERATION_PIXVERSE_TASKS_H
#include "umicom/media_generation/pixverse.h"
#include "umicom/provider_connections/task_store.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_PIXVERSE_TASK_OPERATION "pixverse.video.generate"
/* Pure preparation: derive request identity from the existing immutable plan,
 * including its URL, trace and body. The plan must be an unused creation plan.
 * Connection must describe this adapter: provider pixverse, HTTPS endpoint
 * https://app-api.pixverse.ai/openapi/v2/ and model v6. These settings identify
 * the current adapter; they do not add endpoint/model/timeout configurability.
 * No key, prompt, signed URL or plan bytes are copied into the receipt.
 * Preserve outputs on failure. Host strings use the shared bounded ID grammar. */
UmiStatus UmiPixVerseTaskPrepare(const UmiPixVersePlan *plan,const char *application_id,
    const char *profile_id,const char *local_id,const UmiProviderConnection *connection,
    uint64_t connection_revision,UmiProviderTaskReceipt *out_receipt);
/* Interpret one result returned by execution of this exact, now-consumed plan.
 * A non-OK execution status is returned without changing the receipt. Local
 * timeout/cancel is never a confirmed provider failure. Creation binds the
 * original request digest; status binds the exact positive decimal video ID.
 * Confirmed states delegate to the common task state machine, including no-op
 * and stale-revision rules. This is evidence validation, not authentication.
 * It does not execute requests or persist anything. In-place receipt updates
 * work; other input/output storage must not overlap. Signed URLs stay outside
 * the task record. A successful provider state is not a stored output. */
UmiStatus UmiPixVerseTaskObserve(const UmiProviderTaskReceipt *receipt,uint64_t expected_revision,
    const UmiPixVersePlan *plan,UmiStatus execution_status,const UmiPixVerseResult *result,
    UmiProviderTaskReceipt *out_receipt);
/* Read and interpret through Observe, then commit the corresponding shared
 * store transition with the original expected revision. The second revision
 * check catches an intervening edit. No database transaction spans transport.
 * Preserve output on failure, including a failed commit. A confirmed result
 * must remain owned by the host until this save succeeds or is reconciled;
 * never recreate a paid job because saving its receipt failed. */
UmiStatus UmiPixVerseTaskRecordResult(UmiProviderTasks *store,const char *local_id,
    uint64_t expected_revision,const UmiPixVersePlan *plan,UmiStatus execution_status,
    const UmiPixVerseResult *result,UmiProviderTaskReceipt *out_receipt);
/* Prepare one status check only after the shared recovery binding accepts the
 * application/profile and current connection revision/fingerprint. Require a
 * fresh trace UUID and *out_plan == NULL. No network or credential access.
 * The host must own the recovery worker, reread the persisted task, approve the
 * exact status plan and resolve the matching credential before execution.
 * PREPARED/UNCERTAIN/terminal tasks cannot become new creation requests here.
 * This helper does not provide a worker lease or resume the GTK panel. */
UmiStatus UmiPixVerseTaskStatusPlan(const UmiProviderTaskReceipt *receipt,
    const char *application_id,const char *profile_id,const UmiProviderConnection *connection,
    uint64_t connection_revision,const char *trace_id,UmiPixVersePlan **out_plan);
#ifdef __cplusplus
}
#endif
#endif
