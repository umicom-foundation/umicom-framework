/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_prompt.c
 *
 * PURPOSE:
 *   Centralize immutable workflow request bytes, connection review and task preparation for native ComfyUI clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_prompt.h"
#include "comfy_response_internal.h"
#include "umicom/protocol/json_writer.h"
#include "umicom/base/utf8.h"
#include "umicom/security/secret_provider.h"
#include <stdlib.h>
#include <string.h>

struct UmiComfyPrompt {
    UmiProviderConnection connection;
    UmiComfyPromptReview review;
    char *body;
    /* Retain coordinates in the immutable body so consumers never reparse the
     * envelope with different JSON depth or node-count limits. */
    size_t workflow_offset, workflow_bytes;
};

/* Prompt preparation and node discovery must share one exact saved-origin policy. The reusable response service now owns this check. The former implementation is retained for owner review. */
#if 0
static UmiStatus PromptConnection(const UmiProviderConnection *connection)
{
    UmiStatus status = UmiProviderConnectionValidate(connection);
    if (status != UMI_STATUS_OK) return status;
    if (!connection->enabled || connection->route != UMI_PROVIDER_CONNECTION_LOOPBACK ||
        strcmp(connection->provider_id, "comfyui") != 0)
        return UMI_STATUS_PERMISSION_DENIED;
    /* Validation already proved the fixed literal origin and bounded port.
     * Refuse a saved path rather than guessing whether it is a reverse proxy. */
    if (strchr(connection->endpoint + sizeof("http://127.0.0.1:") - 1U, '/') != NULL)
        return UMI_STATUS_PERMISSION_DENIED;
    return UMI_STATUS_OK;
}
#endif
static UmiStatus PromptConnection(const UmiProviderConnection *connection)
{
    return UmiComfyConnectionCheck(connection);
}

static UmiStatus PromptShape(const UmiJsonTree *tree, const UmiCancellationToken *cancel)
{
    size_t count = UmiJsonTreeCount(tree, 0);
    if (count == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_COMFY_PROMPT_NODE_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (int key = UmiJsonTreeFirst(tree, 0); key >= 0;) {
        if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
        char id[129], type[129];
        UmiStatus status = UmiJsonTreeText(tree, key, id, sizeof(id));
        if (status != UMI_STATUS_OK) return status;
        if (id[0] == '\0') return UMI_STATUS_PARSE_ERROR;
        int node = UmiJsonTreeNext(tree, key), inputs = -1;
        if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
            return UMI_STATUS_PARSE_ERROR;
        status = UmiComfyResponseText(tree, node, "class_type", type, sizeof(type), 0);
        if (status != UMI_STATUS_OK) return status;
        status = UmiComfyResponseMember(tree, node, "inputs", &inputs);
        if (status != UMI_STATUS_OK) return status;
        if (UmiJsonTreeKind(tree, inputs) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
            return UMI_STATUS_PARSE_ERROR;
        key = UmiJsonTreeNext(tree, node);
    }
    return UMI_STATUS_OK;
}

void UmiComfyPromptDestroy(UmiComfyPrompt *prompt)
{
    if (prompt == NULL) return;
    if (prompt->body != NULL) {
        /* A workflow can contain private text and file references. Retire the
         * owned request; caller input, review copies and logs remain host-owned. */
        umi_secret_clear(prompt->body, prompt->review.body_bytes + 1U);
        free(prompt->body);
    }
    umi_secret_clear(prompt, sizeof(*prompt));
    free(prompt);
}

UmiStatus UmiComfyPromptCreate(const UmiProviderConnection *connection,
    uint64_t connection_revision, const UmiComfyPromptDraft *draft,
    const UmiCancellationToken *cancel, UmiComfyPrompt **out)
{
    if (out == NULL || *out != NULL || draft == NULL || connection_revision == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    UmiStatus status = PromptConnection(connection);
    if (status != UMI_STATUS_OK) return status;
    const char *end = memchr(draft->client_id, '\0', sizeof(draft->client_id));
    if (end == NULL || end == draft->client_id ||
        !UmiUtf8Validate((const unsigned char *)draft->client_id, (size_t)(end - draft->client_id), NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiJsonTree *tree = NULL;
    status = UmiComfyResponseOpen(draft->workflow_json, draft->workflow_length,
        UMI_COMFY_PROMPT_MAX_BYTES, cancel, &tree);
    if (status == UMI_STATUS_OK) status = PromptShape(tree, cancel);
    UmiComfyPrompt *candidate = NULL;
    if (status != UMI_STATUS_OK) goto done;
    candidate = calloc(1U, sizeof(*candidate));
    if (candidate == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    candidate->connection = *connection;
    candidate->review.connection_revision = connection_revision;
    candidate->review.node_count = UmiJsonTreeCount(tree, 0);
    memcpy(candidate->review.client_id, draft->client_id, (size_t)(end - draft->client_id) + 1U);
    strcpy(candidate->review.url, connection->endpoint);
    strcat(candidate->review.url, "/prompt");
    status = UmiProviderConnectionFingerprint(connection, candidate->review.connection_sha256);
    if (status != UMI_STATUS_OK) goto done;
    /* Reuse the common string encoder for the envelope; append only validated
     * workflow bytes. Preserving their exact spelling makes review and task
     * identity refer to the same request, without another JSON serializer. */
    char envelope[1024];
    UmiJsonWriter writer;
    UmiJsonWriterInit(&writer, envelope, sizeof(envelope));
    UmiJsonWriterRaw(&writer, "{\"client_id\":");
    UmiJsonWriterString(&writer, draft->client_id);
    UmiJsonWriterRaw(&writer, ",\"prompt\":");
    if (writer.status != UMI_STATUS_OK) { status = writer.status; goto done; }
    candidate->workflow_offset = writer.length;
    candidate->workflow_bytes = draft->workflow_length;
    candidate->review.body_bytes = writer.length + draft->workflow_length + 1U;
    candidate->body = malloc(candidate->review.body_bytes + 1U);
    if (candidate->body == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    memcpy(candidate->body, envelope, writer.length);
    memcpy(candidate->body + writer.length, draft->workflow_json, draft->workflow_length);
    candidate->body[candidate->review.body_bytes - 1U] = '}';
    candidate->body[candidate->review.body_bytes] = '\0';
    UmiSha256 digest;
    UmiSha256Init(&digest);
    status = UmiSha256Update(&digest, candidate->body, candidate->review.body_bytes);
    if (status == UMI_STATUS_OK) status = UmiSha256Final(&digest, candidate->review.request_sha256);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel)) status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK) { *out = candidate; candidate = NULL; }
done:
    UmiComfyPromptDestroy(candidate);
    UmiJsonTreeDestroy(tree);
    return status;
}
UmiStatus UmiComfyPromptInspect(const UmiComfyPrompt *prompt,
    UmiComfyPromptReview *out_review, const char **out_body)
{
    if (prompt == NULL || out_review == NULL || out_body == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_review = prompt->review;
    *out_body = prompt->body;
    return UMI_STATUS_OK;
}
UmiStatus UmiComfyPromptCheckConnection(const UmiComfyPrompt *prompt,
    const UmiProviderConnection *current, uint64_t current_revision)
{
    if (prompt == NULL || current == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = PromptConnection(current);
    if (status != UMI_STATUS_OK) return status;
    if (current_revision != prompt->review.connection_revision) return UMI_STATUS_BUSY;
    unsigned char digest[UMI_SHA256_BYTES];
    status = UmiProviderConnectionFingerprint(current, digest);
    if (status == UMI_STATUS_OK && memcmp(digest, prompt->review.connection_sha256, sizeof(digest)) != 0)
        status = UMI_STATUS_BUSY;
    return status;
}
static UmiStatus PromptScopeId(const char *text, char *out)
{
    if (text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t n = 0U;
    while (n < UMI_PROVIDER_CONNECTION_ID_CAPACITY && text[n] != '\0') ++n;
    if (n == UMI_PROVIDER_CONNECTION_ID_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, text, n + 1U);
    return UMI_STATUS_OK;
}
UmiStatus UmiComfyPromptPrepareTask(const UmiComfyPrompt *prompt,
    const char *application_id, const char *profile_id, const char *local_id,
    UmiProviderTaskReceipt *out)
{
    if (prompt == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiProviderTaskContext context = {0};
    UmiStatus status = PromptScopeId(application_id, context.application_id);
    if (status == UMI_STATUS_OK) status = PromptScopeId(profile_id, context.profile_id);
    if (status == UMI_STATUS_OK) status = PromptScopeId(local_id, context.local_id);
    if (status != UMI_STATUS_OK) return status;
    strcpy(context.operation_id, UMI_COMFY_TASK_OPERATION);
    memcpy(context.request_sha256, prompt->review.request_sha256, sizeof(context.request_sha256));
    return UmiProviderTaskPrepare(&context, &prompt->connection, prompt->review.connection_revision, out);
}

UmiStatus UmiComfyPromptWorkflow(const UmiComfyPrompt *prompt,
    const char **out_json, size_t *out_bytes)
{
    if (!prompt || !out_json || !out_bytes) return UMI_STATUS_INVALID_ARGUMENT;
    *out_json = prompt->body + prompt->workflow_offset;
    *out_bytes = prompt->workflow_bytes;
    return UMI_STATUS_OK;
}
