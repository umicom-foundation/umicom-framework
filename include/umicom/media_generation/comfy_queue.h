/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/comfy_queue.h
 *
 * PURPOSE:
 *   Expose owned queue observations with exact numeric tokens and no retained workflow contents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_MEDIA_GENERATION_COMFY_QUEUE_H
#define UMICOM_MEDIA_GENERATION_COMFY_QUEUE_H
#include "umicom/provider_connections/connections.h"
#include "umicom/provider_connections/tasks.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_COMFY_QUEUE_MAX_BYTES (4U * 1024U * 1024U)
#define UMI_COMFY_QUEUE_LIMIT 4096U
#define UMI_COMFY_QUEUE_NUMBER_CAPACITY 129U
typedef enum UmiComfyQueueState {
    UMI_COMFY_QUEUE_RUNNING=1, UMI_COMFY_QUEUE_PENDING=2
} UmiComfyQueueState;
typedef struct UmiComfyQueueEntry {
    char prompt_id[UMI_PROVIDER_TASK_REMOTE_ID_CAPACITY];
    /* Exact JSON number spelling, including sign/fraction/exponent. This is
     * an opaque scheduling value, not an ordinal, duration or completion %. */
    char number_json[UMI_COMFY_QUEUE_NUMBER_CAPACITY];
    UmiComfyQueueState state;
} UmiComfyQueueEntry;
typedef struct UmiComfyQueueReview {
    size_t running_count, pending_count, response_bytes;
    uint64_t connection_revision;
    unsigned char connection_sha256[UMI_SHA256_BYTES], response_sha256[UMI_SHA256_BYTES];
} UmiComfyQueueReview;
typedef struct UmiComfyQueue UmiComfyQueue;
/* Observe host-supplied GET /queue JSON. Reuse the saved enabled ComfyUI
 * loopback-origin contract; a nonzero revision and fingerprint label this
 * observation, not an authenticated response or a live server connection.
 * No network, queue mutation, generation or interruption occurs here.
 *
 * Require queue_running and queue_pending arrays. Each entry has at least five
 * fields: number, nonempty prompt ID, prompt object, extra-data object, and an
 * array of nonempty output-node strings. Later fields and root extensions are
 * opaque JSON. All object keys and decoded prompt IDs must be unique, including
 * IDs repeated across running and pending. IDs are case-sensitive UTF-8 text,
 * not normalized or assumed UUIDs. Number grammar is checked but never converted
 * to floating point; large exponents remain opaque valid JSON spellings.
 *
 * Keep running then pending in response order. Pending order is not execution
 * rank; absence does not imply completion or cancellation. Copy only IDs,
 * numbers, category, counts and digests; clear temporary workflow/extra data.
 * Limits: 4 MiB, 65536 JSON nodes, depth 32, 4096 total entries, 128 UTF-8 ID
 * bytes and 128 number-token bytes. Empty lists are valid. Initialize *out=NULL;
 * every failure preserves it. Cancellation is cooperative; sorting/hash/copy
 * operations are not interruptible. Caller input stays immutable during call. */
UmiStatus UmiComfyQueueCreate(const UmiProviderConnection *connection,
    uint64_t connection_revision,const void *bytes,size_t length,
    const UmiCancellationToken *cancel,UmiComfyQueue **out);
void UmiComfyQueueDestroy(UmiComfyQueue *queue);
/* Serialize reads and destruction. Outputs are copied and unchanged on failure;
 * inputs/outputs must not overlap owned storage. Count(NULL) returns zero. */
size_t UmiComfyQueueCount(const UmiComfyQueue *queue);
UmiStatus UmiComfyQueueInspect(const UmiComfyQueue *queue,UmiComfyQueueReview *out);
UmiStatus UmiComfyQueueAt(const UmiComfyQueue *queue,size_t index,UmiComfyQueueEntry *out);
UmiStatus UmiComfyQueueFind(const UmiComfyQueue *queue,const char *prompt_id,size_t *out_index);
/* BUSY means a different saved revision/content. A match does not establish
 * response freshness, authorization to cancel, or ownership of a remote job. */
UmiStatus UmiComfyQueueCheckConnection(const UmiComfyQueue *queue,
    const UmiProviderConnection *current,uint64_t current_revision);
#ifdef __cplusplus
}
#endif
#endif
