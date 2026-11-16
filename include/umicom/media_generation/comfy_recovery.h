/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/comfy_recovery.h
 *
 * PURPOSE:
 *   Own bounded submission and history observations for identity-bound ComfyUI task recovery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_MEDIA_GENERATION_COMFY_RECOVERY_H
#define UMICOM_MEDIA_GENERATION_COMFY_RECOVERY_H
#include "umicom/media_generation/comfy_events.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_COMFY_HISTORY_MAX_BYTES (4U * 1024U * 1024U)
#define UMI_COMFY_HISTORY_IMAGE_LIMIT 1024U

typedef struct UmiComfySubmission {
    char prompt_id[UMI_PROVIDER_TASK_REMOTE_ID_CAPACITY];
    /* Preserve the queue number's exact JSON spelling, including fractional or
     * negative values. It is not an estimated wait, task revision or array index. */
    char queue_number[65];
    size_t node_error_count;
} UmiComfySubmission;
/* Accept one HTTP 200 /prompt reply. A prompt ID, numeric queue number and
 * node_errors object are required. Nonempty node_errors can accompany an
 * accepted partial workflow; report the count instead of claiming all nodes
 * passed. A non-null error member rejects the observation. Other HTTP statuses
 * return UNAVAILABLE without changing out or deciding whether a job exists.
 * Limits: 1 MiB, 65536 JSON nodes, depth 32. No request, retry, storage or logging occurs. */
UmiStatus UmiComfySubmissionDecode(unsigned http_status, const void *bytes, size_t length,
    const UmiCancellationToken *cancel, UmiComfySubmission *out);
/* Confirm the parsed ID through shared task admission. The host must have
 * durably saved SUBMISSION_UNCERTAIN before it sent the request, validate the
 * response's transport/connection binding and save this receipt by revision.
 * Cancellation before decoding preserves uncertainty. Reconcile a response
 * received during stop-waiting separately; never resend to obtain an ID.
 * Refusals preserve out; in-place receipt updates are supported. */
UmiStatus UmiComfyTaskConfirmJson(const UmiProviderTaskReceipt *receipt,
    uint64_t expected_revision, unsigned http_status, const void *bytes, size_t length,
    const UmiCancellationToken *cancel, UmiProviderTaskReceipt *out);

typedef enum UmiComfyHistoryState {
    UMI_COMFY_HISTORY_UNKNOWN = 0,
    UMI_COMFY_HISTORY_SUCCEEDED,
    UMI_COMFY_HISTORY_FAILED
} UmiComfyHistoryState;
typedef enum UmiComfyImageFolder {
    UMI_COMFY_IMAGE_OUTPUT = 1,
    UMI_COMFY_IMAGE_TEMP,
    UMI_COMFY_IMAGE_INPUT
} UmiComfyImageFolder;
typedef struct UmiComfyImageReference {
    char node_id[UMI_PROVIDER_TASK_REMOTE_ID_CAPACITY];
    char filename[513], subfolder[513];
    UmiComfyImageFolder folder;
} UmiComfyImageReference;
typedef struct UmiComfyHistory UmiComfyHistory;
/* Own an immutable observation of one /history/{prompt_id} reply. The selected
 * ID is 1..128 UTF-8 bytes and compared exactly; it is not used as a URL here.
 * Missing selected entry -> NOT_FOUND, never queued, running or safe-to-retry.
 * Missing/null status or success with completed:false -> UNKNOWN. A success
 * with completed:true is SUCCEEDED; error with completed:false is FAILED.
 * Contradictory error/completed:true is refused. Unrecognized status strings
 * return NOT_IMPLEMENTED. Only SUCCEEDED exposes images from outputs.*.images.
 * Other output kinds are validated JSON but intentionally not interpreted.
 *
 * Limits: 4 MiB, 65536 JSON nodes, depth 32, 1024 image references, node ID 128,
 * filename/subfolder 512 UTF-8 bytes. All object keys must be unique. Order is
 * source node order followed by image order; duplicate image references remain
 * distinct occurrences. Failure clears *out; do not pass an occupied owner.
 * The raw response copy is cleared by the shared JSON owner before release.
 * No I/O or authenticity claim. Cancellation is cooperative, not a remote stop. */
UmiStatus UmiComfyHistoryCreate(const void *bytes, size_t length, const char *prompt_id,
    const UmiCancellationToken *cancel, UmiComfyHistory **out);
void UmiComfyHistoryDestroy(UmiComfyHistory *history);
UmiComfyHistoryState UmiComfyHistoryStateOf(const UmiComfyHistory *history);
size_t UmiComfyHistoryImageCount(const UmiComfyHistory *history);
/* Metadata only: names can contain provider-specific paths and are never safe
 * local filenames merely because parsing succeeded. A later reviewed importer
 * must validate containment, encode URL query parameters, validate media bytes
 * and apply Umicom branding. This API never downloads or claims output stored.
 * Copies remain caller-owned; destroy does not erase copies. Failure preserves out. */
UmiStatus UmiComfyHistoryImageAt(const UmiComfyHistory *history, size_t index,
    UmiComfyImageReference *out);
/* Match the immutable history ID to the confirmed receipt and delegate terminal
 * observations to shared task rules. UNKNOWN returns UNAVAILABLE unchanged.
 * Host verifies the current connection and expected persistent revision first.
 * This neither persists a transition nor calls UmiProviderTaskRecordOutput. */
UmiStatus UmiComfyTaskObserveHistory(const UmiProviderTaskReceipt *receipt,
    uint64_t expected_revision, const UmiComfyHistory *history, UmiProviderTaskReceipt *out);
#ifdef __cplusplus
}
#endif
#endif
