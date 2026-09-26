/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/extractive_provider.c
 *
 * PURPOSE:
 *   Provide a clearly labelled offline reference preview without pretending to run a language model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai_workspace/providers.h"
#include "workspace_internal.h"
#include <stdio.h>
#include <string.h>

static UmiStatus AwAppend(char *out, size_t capacity, const char *text)
{
    size_t used = strlen(out), length = strlen(text);
    if (used >= capacity || length >= capacity - used) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out + used, text, length + 1U); return UMI_STATUS_OK;
}
/* Read only small decimal offsets, with a bound before multiplication. The
 * numbers describe bytes in one already bounded canonical message. */
static bool AwReferenceNumber(const char **cursor, size_t *out)
{
    const char *text = *cursor; size_t value = 0U;
    if (*text < '0' || *text > '9') return false;
    while (*text >= '0' && *text <= '9') {
        size_t digit = (size_t)(*text++ - '0');
        if (value > (UMI_AI_TEXT_CAPACITY - digit) / 10U) return false;
        value = value * 10U + digit;
    }
    *cursor = text; *out = value; return true;
}
static UmiStatus AwPreviewGenerate(void *instance, const UmiAiRequest *request, UmiAiResponse *response)
{
    (void)instance;
    if (request == NULL || response == NULL || request->message_count < 2U || request->message_count > UMI_AI_MAX_MESSAGES ||
        !AwTextValid(request->request_id, sizeof(request->request_id), false) ||
        !AwTextValid(request->model_id, sizeof(request->model_id), false) || request->allow_tools)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(request->model_id, "extractive-preview") != 0) return UMI_STATUS_NOT_FOUND;
    UmiAiResponse result = {0}; size_t references = 0U;
    (void)AwTextCopy(result.request_id, sizeof(result.request_id), request->request_id, false);
    (void)AwTextCopy(result.provider_id, sizeof(result.provider_id), "umicom.extractive-preview", false);
    (void)AwTextCopy(result.model_id, sizeof(result.model_id), request->model_id, false);
    (void)AwTextCopy(result.text, sizeof(result.text),
        "Extractive preview - copied reference text, not a language-model answer.\n", false);
    /* Only the framed messages between the system message and final user
     * request are sources. A question that starts with "Source [S1]" is still
     * a question and cannot create a reference. */
    for (size_t i = 1U; i + 1U < request->message_count; ++i) {
        const char *text = request->messages[i].text;
        if (!AwTextValid(text, UMI_AI_TEXT_CAPACITY, true)) return UMI_STATUS_INVALID_ARGUMENT;
        if (++references > UMI_AI_WORKSPACE_MAX_EVIDENCE) return UMI_STATUS_CAPACITY_EXCEEDED;
        const char *name = request->messages[i].name;
        const char *prefix = "umicom.reference.";
        size_t start = 0U, length = 0U, textLength = strlen(text);
        if (!AwTextValid(name, sizeof(request->messages[i].name), false) ||
            strncmp(name, prefix, strlen(prefix)) != 0) return UMI_STATUS_PARSE_ERROR;
        const char *cursor = name + strlen(prefix);
        if (!AwReferenceNumber(&cursor, &start) || *cursor++ != '.' ||
            !AwReferenceNumber(&cursor, &length) || *cursor != '\0' || start > textLength ||
            length > textLength - start) return UMI_STATUS_PARSE_ERROR;
        const char *begin = text + start;
        bool shortened = length > 700U;
        if (shortened) {
            length = 700U;
            while (length > 0U && ((unsigned char)begin[length] & 0xC0U) == 0x80U) --length;
        }
        char excerpt[701], label[64];
        memcpy(excerpt, begin, length); excerpt[length] = '\0';
        (void)snprintf(label, sizeof(label), "\n[S%zu]\n", references);
        UmiStatus status = AwAppend(result.text, sizeof(result.text), label);
        if (status == UMI_STATUS_OK) status = AwAppend(result.text, sizeof(result.text), excerpt);
        if (status == UMI_STATUS_OK && shortened) status = AwAppend(result.text, sizeof(result.text), "\n[Excerpt shortened; inspect the full saved passage.]\n");
        if (status != UMI_STATUS_OK) return status;
    }
    if (references == 0U) {
        UmiStatus status = AwAppend(result.text, sizeof(result.text),
            "No reference passages were supplied. Choose Grounded draft to inspect retrieved sources, "
            "or select a configured local model for actual text generation.");
        if (status != UMI_STATUS_OK) return status;
    }
    result.finish_reason = UMI_AI_FINISH_STOP; *response = result; return UMI_STATUS_OK;
}
UmiStatus UmiAiWorkspaceExtractiveProviderCreate(UmiAiProvider *outProvider)
{
    if (outProvider == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(outProvider, 0, sizeof(*outProvider)); outProvider->structure_size = sizeof(*outProvider);
    outProvider->abi_version = 1U; outProvider->provider_id = "umicom.extractive-preview";
    outProvider->kind = UMI_AI_PROVIDER_LOCAL; outProvider->generate = AwPreviewGenerate;
    return UMI_STATUS_OK;
}
static UmiStatus AwCapacityTool(const char *arguments, char *out, size_t capacity, void *context)
{
    (void)context;
    if (arguments == NULL || strcmp(arguments, "{}") != 0 || out == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    char text[256];
    int written = snprintf(text, sizeof(text),
        "{\"collections\":%u,\"source_passages\":%u,\"jobs\":%u,\"evidence_per_job\":%u,\"passage_bytes_including_nul\":%u}",
        UMI_AI_WORKSPACE_MAX_COLLECTIONS, UMI_AI_WORKSPACE_MAX_SOURCES, UMI_AI_WORKSPACE_MAX_JOBS,
        UMI_AI_WORKSPACE_MAX_EVIDENCE, UMI_AI_WORKSPACE_PASSAGE_CAPACITY);
    if (written < 0 || (size_t)written >= sizeof(text)) return UMI_STATUS_INTERNAL_ERROR;
    return AwTextCopy(out, capacity, text, false);
}
UmiStatus UmiAiWorkspaceRegisterCapacityTool(UmiAiRuntime *runtime)
{
    UmiAiTool tool = {0};
    if (runtime == NULL || runtime->tools.count > UMI_AI_MAX_TOOLS) return UMI_STATUS_INVALID_ARGUMENT;
    (void)AwTextCopy(tool.tool_id, sizeof(tool.tool_id), "umicom.workspace.capacities", false);
    (void)AwTextCopy(tool.description, sizeof(tool.description), "Read the compiled AI workspace limits. Does not modify sources or files.", false);
    (void)AwTextCopy(tool.permission, sizeof(tool.permission), "workspace.read-limits", false);
    tool.invoke = AwCapacityTool;
    return umi_ai_tool_registry_add(&runtime->tools, &tool);
}
