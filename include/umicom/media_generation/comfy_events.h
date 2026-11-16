/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/comfy_events.h
 *
 * PURPOSE:
 *   Decode bounded ComfyUI job observations and bind terminal evidence to the shared provider task lifecycle.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_MEDIA_GENERATION_COMFY_EVENTS_H
#define UMICOM_MEDIA_GENERATION_COMFY_EVENTS_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/provider_connections/tasks.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_COMFY_TASK_OPERATION "comfy.workflow.submit"
#define UMI_COMFY_EVENT_MAX_BYTES (1024U * 1024U)
typedef enum UmiComfyEventKind {
    UMI_COMFY_EVENT_QUEUE = 1,
    UMI_COMFY_EVENT_START,
    UMI_COMFY_EVENT_PROGRESS,
    UMI_COMFY_EVENT_EXECUTING,
    UMI_COMFY_EVENT_UI_OUTPUT,
    UMI_COMFY_EVENT_CACHED,
    UMI_COMFY_EVENT_SUCCESS,
    UMI_COMFY_EVENT_ERROR,
    UMI_COMFY_EVENT_INTERRUPTED
} UmiComfyEventKind;
typedef struct UmiComfyEvent {
    UmiComfyEventKind kind;
    char prompt_id[UMI_PROVIDER_TASK_REMOTE_ID_CAPACITY];
    char node_id[UMI_PROVIDER_TASK_REMOTE_ID_CAPACITY];
    uint64_t value, maximum, queue_remaining;
    int node_is_null;
} UmiComfyEvent;
/* Decode one JSON text message, not a binary preview or WebSocket frame.
 * The response must use type/data, unique decoded object keys, bounded UTF-8
 * IDs and protocol-appropriate fields. Unknown types return NOT_IMPLEMENTED.
 * Limits: 1 MiB, 32768 JSON nodes, depth 32. Numbers use exact integer spelling
 * in 0..INT64_MAX. Progress requires value <= max (zero/zero is allowed).
 * Queue status is global. executing with node:null is represented explicitly;
 * it is not converted to success. executed is a UI output observation.
 * Unknown fields are validated but not retained; no raw errors/outputs escape.
 * No network, provider, filesystem or process action occurs. Cancellation is
 * cooperative. Every refusal preserves out; borrowed inputs live for the call.
 */
UmiStatus UmiComfyEventDecode(const void *bytes, size_t length,
    const UmiCancellationToken *cancel, UmiComfyEvent *out);
/* Decode and apply only a confirmed task's matching prompt ID. Receipt must be
 * for provider comfyui and operation UMI_COMFY_TASK_OPERATION. Caller verifies
 * the socket destination and current connection binding before passing evidence;
 * parsing does not authenticate a sender. expected_revision must still match.
 * START/PROGRESS -> running, SUCCESS -> succeeded, ERROR/INTERRUPTED -> failed.
 * Queue/UI/cached/executing observations return UNAVAILABLE without mutation.
 * A wrong prompt returns NOT_FOUND. Shared task rules reject stale/backward
 * transitions. This does not persist: save by expected revision before exposing
 * the new state. In-place receipt update works; all failure outputs unchanged.
 * Local timeout or cancellation is not a server interruption event. No generic
 * cancel request, retry, output retrieval or terminal success is synthesized.
 */
UmiStatus UmiComfyTaskObserveJson(const UmiProviderTaskReceipt *receipt,
    uint64_t expected_revision, const void *bytes, size_t length,
    const UmiCancellationToken *cancel, UmiProviderTaskReceipt *out);
#ifdef __cplusplus
}
#endif
#endif
