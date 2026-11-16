/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_history.c
 *
 * PURPOSE:
 *   Recover one confirmed ComfyUI history observation without inventing task completion or importing provider paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "comfy_response_internal.h"
#include "umicom/security/secrets.h"
#include <stdlib.h>
#include <string.h>
struct UmiComfyHistory {
    char prompt_id[UMI_PROVIDER_TASK_REMOTE_ID_CAPACITY];
    UmiComfyHistoryState state;
    size_t count;
    UmiComfyImageReference *images;
};
static UmiStatus state_read(const UmiJsonTree *tree, int entry, UmiComfyHistoryState *out)
{
    int node = -1;
    UmiStatus status = UmiJsonTreeMember(tree, entry, "status", &node);
    if (status == UMI_STATUS_NOT_FOUND || (status == UMI_STATUS_OK && UmiJsonTreeIsNull(tree, node))) {
        *out = UMI_COMFY_HISTORY_UNKNOWN; return UMI_STATUS_OK;
    }
    if (status != UMI_STATUS_OK) return status;
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) return UMI_STATUS_PARSE_ERROR;
    char name[32]; int completed = -1, value = 0;
    status = UmiComfyResponseText(tree, node, "status_str", name, sizeof(name), 0);
    if (status == UMI_STATUS_OK) status = UmiComfyResponseMember(tree, node, "completed", &completed);
    if (status == UMI_STATUS_OK) status = UmiJsonTreeBoolean(tree, completed, &value);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(name, "success") == 0) {
        *out = value ? UMI_COMFY_HISTORY_SUCCEEDED : UMI_COMFY_HISTORY_UNKNOWN; return UMI_STATUS_OK;
    }
    if (strcmp(name, "error") == 0) {
        if (value) return UMI_STATUS_PARSE_ERROR;
        *out = UMI_COMFY_HISTORY_FAILED; return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_IMPLEMENTED;
}
static UmiStatus image_read(const UmiJsonTree *tree, int object, const char *node_id,
    UmiComfyImageReference *out)
{
    if (UmiJsonTreeKind(tree, object) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) return UMI_STATUS_PARSE_ERROR;
    UmiComfyImageReference staged = {0}; char type[16];
    UmiStatus status = UmiComfyResponseText(tree, object, "filename", staged.filename, sizeof(staged.filename), 0);
    if (status == UMI_STATUS_OK) status = UmiComfyResponseText(tree, object, "subfolder", staged.subfolder, sizeof(staged.subfolder), 1);
    if (status == UMI_STATUS_OK) status = UmiComfyResponseText(tree, object, "type", type, sizeof(type), 0);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(type, "output") == 0) staged.folder = UMI_COMFY_IMAGE_OUTPUT;
    else if (strcmp(type, "temp") == 0) staged.folder = UMI_COMFY_IMAGE_TEMP;
    else if (strcmp(type, "input") == 0) staged.folder = UMI_COMFY_IMAGE_INPUT;
    else return UMI_STATUS_NOT_IMPLEMENTED;
    strcpy(staged.node_id, node_id);
    *out = staged; return UMI_STATUS_OK;
}
/* Two bounded passes avoid reallocating on every image. The immutable tree
 * guarantees that the count and copy passes see the same provider observation. */
static UmiStatus images_read(const UmiJsonTree *tree, int outputs, const UmiCancellationToken *cancel,
    UmiComfyImageReference *images, size_t capacity, size_t *out_count)
{
    size_t count = 0;
    for (int key = UmiJsonTreeFirst(tree, outputs); key >= 0;) {
        if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
        int object = UmiJsonTreeNext(tree, key);
        if (UmiJsonTreeKind(tree, object) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) return UMI_STATUS_PARSE_ERROR;
        int list = -1;
        UmiStatus status = UmiJsonTreeMember(tree, object, "images", &list);
        if (status != UMI_STATUS_NOT_FOUND) {
            if (status != UMI_STATUS_OK) return status;
            if (UmiJsonTreeKind(tree, list) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY) return UMI_STATUS_PARSE_ERROR;
            size_t amount = UmiJsonTreeCount(tree, list);
            if (amount > UMI_COMFY_HISTORY_IMAGE_LIMIT - count) return UMI_STATUS_CAPACITY_EXCEEDED;
            char node_id[UMI_PROVIDER_TASK_REMOTE_ID_CAPACITY];
            status = UmiJsonTreeText(tree, key, node_id, sizeof(node_id));
            if (status != UMI_STATUS_OK) return status;
            if (node_id[0] == '\0') return UMI_STATUS_PARSE_ERROR;
            for (int image = UmiJsonTreeFirst(tree, list); image >= 0; image = UmiJsonTreeNext(tree, image)) {
                if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
                UmiComfyImageReference staged;
                status = image_read(tree, image, node_id, &staged);
                if (status != UMI_STATUS_OK) return status;
                if (images != NULL) {
                    if (count >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
                    images[count] = staged;
                }
                ++count;
            }
        }
        key = UmiJsonTreeNext(tree, object);
    }
    *out_count = count; return UMI_STATUS_OK;
}
UmiStatus UmiComfyHistoryCreate(const void *bytes, size_t length, const char *prompt_id,
    const UmiCancellationToken *cancel, UmiComfyHistory **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (prompt_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t id_length = 0;
    while (id_length < UMI_PROVIDER_TASK_REMOTE_ID_CAPACITY && prompt_id[id_length] != '\0') ++id_length;
    if (id_length == 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (id_length == UMI_PROVIDER_TASK_REMOTE_ID_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiJsonTree *tree = NULL;
    UmiStatus status = UmiComfyResponseOpen(bytes, length, UMI_COMFY_HISTORY_MAX_BYTES, cancel, &tree);
    if (status != UMI_STATUS_OK) return status;
    UmiComfyHistory *history = NULL;
    int entry = -1, outputs = -1;
    status = UmiJsonTreeMember(tree, 0, prompt_id, &entry);
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, entry) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) {
        history = calloc(1U, sizeof(*history));
        if (history == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status == UMI_STATUS_OK) {
        memcpy(history->prompt_id, prompt_id, id_length + 1U);
        status = state_read(tree, entry, &history->state);
    }
    if (status == UMI_STATUS_OK) status = UmiComfyResponseMember(tree, entry, "outputs", &outputs);
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, outputs) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK && history->state == UMI_COMFY_HISTORY_SUCCEEDED) {
        status = images_read(tree, outputs, cancel, NULL, 0, &history->count);
        if (status == UMI_STATUS_OK && history->count > 0) {
            history->images = calloc(history->count, sizeof(*history->images));
            if (history->images == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
            else {
                size_t copied = 0;
                status = images_read(tree, outputs, cancel, history->images, history->count, &copied);
                if (status == UMI_STATUS_OK && copied != history->count) status = UMI_STATUS_INVALID_STATE;
            }
        }
    }
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel)) status = UMI_STATUS_CANCELLED;
    UmiJsonTreeDestroy(tree);
    if (status == UMI_STATUS_OK) *out = history;
    else UmiComfyHistoryDestroy(history);
    return status;
}
void UmiComfyHistoryDestroy(UmiComfyHistory *history)
{
    if (history == NULL) return;
    if (history->images != NULL) {
        umi_secret_clear(history->images, history->count * sizeof(*history->images));
        free(history->images);
    }
    umi_secret_clear(history, sizeof(*history)); free(history);
}
UmiComfyHistoryState UmiComfyHistoryStateOf(const UmiComfyHistory *history)
{ return history == NULL ? UMI_COMFY_HISTORY_UNKNOWN : history->state; }
size_t UmiComfyHistoryImageCount(const UmiComfyHistory *history)
{ return history == NULL ? 0 : history->count; }
UmiStatus UmiComfyHistoryImageAt(const UmiComfyHistory *history, size_t index, UmiComfyImageReference *out)
{
    if (history == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= history->count) return UMI_STATUS_NOT_FOUND;
    *out = history->images[index]; return UMI_STATUS_OK;
}
UmiStatus UmiComfyTaskObserveHistory(const UmiProviderTaskReceipt *receipt,
    uint64_t expected_revision, const UmiComfyHistory *history, UmiProviderTaskReceipt *out)
{
    if (history == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiComfyReceiptCheck(receipt, expected_revision);
    if (status != UMI_STATUS_OK) return status;
    if (receipt->remote_id[0] == '\0') return UMI_STATUS_INVALID_STATE;
    if (strcmp(receipt->remote_id, history->prompt_id) != 0) return UMI_STATUS_NOT_FOUND;
    UmiProviderTaskEvent event;
    if (history->state == UMI_COMFY_HISTORY_SUCCEEDED) event = UMI_PROVIDER_TASK_OBSERVE_SUCCESS;
    else if (history->state == UMI_COMFY_HISTORY_FAILED) event = UMI_PROVIDER_TASK_OBSERVE_FAILURE;
    else return UMI_STATUS_UNAVAILABLE;
    return UmiProviderTaskAdvance(receipt, expected_revision, event, out);
}
