/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media_generation/comfy_prompt.h
 *
 * PURPOSE:
 *   Prepare owned API-format ComfyUI requests and bind their exact bytes to shared task receipts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_MEDIA_GENERATION_COMFY_PROMPT_H
#define UMICOM_MEDIA_GENERATION_COMFY_PROMPT_H
#include "umicom/media_generation/comfy_recovery.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_COMFY_PROMPT_MAX_BYTES (1024U * 1024U)
#define UMI_COMFY_PROMPT_NODE_LIMIT 1024U

typedef struct UmiComfyPromptDraft {
    const void *workflow_json;
    size_t workflow_length;
    char client_id[129];
} UmiComfyPromptDraft;
typedef struct UmiComfyPrompt UmiComfyPrompt;
typedef struct UmiComfyPromptReview {
    char url[UMI_PROVIDER_CONNECTION_ENDPOINT_CAPACITY];
    char client_id[129];
    size_t node_count, body_bytes;
    uint64_t connection_revision;
    unsigned char connection_sha256[UMI_SHA256_BYTES];
    unsigned char request_sha256[UMI_SHA256_BYTES];
} UmiComfyPromptReview;
/* Pure preparation for provider_id=comfyui and an enabled saved loopback
 * connection. Endpoint must be exactly http://127.0.0.1:<port>, without a path;
 * use the connection service's allowed port/timeout range. Revision is nonzero.
 * Loopback describes the destination, not the effects of server custom nodes.
 *
 * Copy one API-format workflow object with 1..1024 node members. Each node ID
 * and class_type is 1..128 UTF-8 bytes; inputs must be an object. Unknown fields
 * and custom input values are retained exactly. All object keys must be unique.
 * Byte/node/depth limits are 1 MiB/65536/32. This does not inspect link targets,
 * output types, model files, node availability or server extensions. Use the
 * separate typed workflow preflight for host-declared graphs; the server still
 * validates its actual installed node schemas. Editor layout JSON is not an API
 * prompt. There is no network call, code loading or promise of generation.
 *
 * client_id is 1..128 UTF-8 bytes, escaped by the shared JSON writer. It routes
 * observations; it is not authentication or a server task ID. The request body
 * is {"client_id":...,"prompt":...}; the prompt's original JSON spelling is
 * retained. No private fields are added or logged. Initialize *out=NULL.
 * Failures preserve an occupied output, otherwise leave it NULL. Borrow inputs
 * only during this call. Cancellation is cooperative between nodes and phases. */
UmiStatus UmiComfyPromptCreate(const UmiProviderConnection *connection,
    uint64_t connection_revision, const UmiComfyPromptDraft *draft,
    const UmiCancellationToken *cancel, UmiComfyPrompt **out);
/* Review copies metadata; body borrows immutable, NUL-terminated bytes until
 * Destroy. Both outputs are required and preserved on failure. Serialize access
 * and destruction. Inputs and outputs must not overlap. Review is not approval. */
UmiStatus UmiComfyPromptInspect(const UmiComfyPrompt *prompt,
    UmiComfyPromptReview *out_review, const char **out_body);
/* Borrow the original workflow bytes inside the owned request, including its
 * original whitespace. The span is not NUL-terminated; it excludes the request
 * envelope. Valid until Destroy, immutable, and outputs preserved on refusal.
 * Reuse the workflow's original byte/depth/node limits when inspecting it. */
UmiStatus UmiComfyPromptWorkflow(const UmiComfyPrompt *prompt,
    const char **out_json, size_t *out_bytes);
void UmiComfyPromptDestroy(UmiComfyPrompt *prompt);
/* Check saved revision plus all canonical settings immediately before a later
 * effect. BUSY means re-read and review, never silently retarget. Does not read a
 * store, acquire a lease, verify a running server or authorize the request. */
UmiStatus UmiComfyPromptCheckConnection(const UmiComfyPrompt *prompt,
    const UmiProviderConnection *current, uint64_t current_revision);
/* Construct PREPARED using the exact request digest and immutable connection.
 * IDs follow the existing provider task grammar; each is a terminated string
 * shorter than UMI_PROVIDER_CONNECTION_ID_CAPACITY. Outputs are preserved on
 * refusal. No persistence or transport occurs. The host must create the receipt
 * in its scoped store, acquire shared task ownership, then durably save
 * SUBMISSION_UNCERTAIN before it sends. Never resubmit an uncertain receipt. */
UmiStatus UmiComfyPromptPrepareTask(const UmiComfyPrompt *prompt,
    const char *application_id, const char *profile_id, const char *local_id,
    UmiProviderTaskReceipt *out);
#ifdef __cplusplus
}
#endif
#endif
