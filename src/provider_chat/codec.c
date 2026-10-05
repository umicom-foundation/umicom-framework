/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/provider_chat/codec.c
 * PURPOSE: Encode the reviewed user message and publish only complete, bounded, non-actionable replies.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/ai/mcp/json.h"
#include "umicom/language_runtime/json_document.h"
#include "umicom/language_runtime/json_text.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Permit ordinary multiline text. Complete JSON parsing later checks UTF-8;
 * rejecting other ASCII controls also keeps review and result widgets legible. */
static bool TextValid(const char *text, size_t capacity, bool required)
{
    if (text == NULL)
        return false;
    bool visible = false;
    for (size_t i = 0U; i < capacity; ++i)
    {
        unsigned char c = (unsigned char)text[i];
        if (c == 0U)
            return !required || visible;
        if ((c < 0x20U && c != '\t' && c != '\r' && c != '\n') || c == 0x7fU)
            return false;
        if (c > 0x20U)
            visible = true;
    }
    return false;
}
UmiStatus UmiProviderChatEncode(UmiProviderChatPlan *plan, const char *prompt, const char *context)
{
    if (plan == NULL || !TextValid(prompt, UMI_PROVIDER_CHAT_PROMPT_CAPACITY, true) ||
        !TextValid(context, UMI_PROVIDER_CHAT_CONTEXT_CAPACITY, false))
        return UMI_STATUS_INVALID_ARGUMENT;
    int length = snprintf(plan->input, sizeof(plan->input), "%s%s%s", prompt,
                          context[0] != '\0' ? "\n\nContext supplied by the user:\n" : "", context);
    if (length < 0 || (size_t)length >= sizeof(plan->input))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char *quoted = calloc(UMI_CONNECTION_CHECK_BODY_CAPACITY, 1U);
    UmiLanguageRuntimeJsonDocument *doc = malloc(sizeof(*doc));
    if (quoted == NULL || doc == NULL)
    {
        free(quoted);
        free(doc);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiStatus status = umi_ai_mcp_json_escape_string(plan->input, quoted, UMI_CONNECTION_CHECK_BODY_CAPACITY);
    if (status == UMI_STATUS_OK)
    {
        /* One visible user message, no implicit system prompt or history.
         * Omit temperature so models with restricted sampling still work. */
        if (plan->responses)
            length = snprintf(plan->body, sizeof(plan->body),
                              "{\"model\":\"%s\",\"input\":%s,\"max_output_tokens\":%" PRIu32
                              ",\"store\":false,\"stream\":false,\"tools\":[],\"tool_choice\":\"none\"}",
                              plan->connection.connection.model, quoted, plan->output_limit);
        else
            length =
                snprintf(plan->body, sizeof(plan->body),
                         "{\"model\":\"%s\",\"messages\":[{\"role\":\"user\",\"content\":%s}],\"%s\":%" PRIu32
                         ",\"store\":false,\"stream\":false}",
                         plan->connection.connection.model, quoted,
                         plan->connection.requires_credential ? "max_completion_tokens" : "max_tokens",
                         plan->output_limit);
        if (length < 0 || (size_t)length >= sizeof(plan->body))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else
            status = UmiLanguageRuntimeJsonParseComplete(plan->body, doc);
    }
    umi_secret_clear(quoted, UMI_CONNECTION_CHECK_BODY_CAPACITY);
    free(quoted);
    free(doc);
    return status;
}
/* -1 means absent; -2 means malformed or duplicated. Optional fields must not
 * confuse a duplicate with absence, including escaped spellings of the name. */
static int Field(const UmiLanguageRuntimeJsonDocument *doc, int object, const char *name)
{
    if (object < 0 || doc->tokens[object].type != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        return -2;
    int found = -1;
    size_t count = umi_language_runtime_json_object_count(doc, object);
    for (size_t i = 0U; i < count; ++i)
    {
        int key, value;
        char name_text[64];
        if (umi_language_runtime_json_object_entry_at(doc, object, i, &key, &value) != UMI_STATUS_OK)
            return -2;
        UmiStatus status = UmiLanguageRuntimeJsonText(doc, key, name_text, sizeof(name_text));
        if (status == UMI_STATUS_CAPACITY_EXCEEDED)
            continue;
        if (status != UMI_STATUS_OK)
            return -2;
        if (strcmp(name_text, name) == 0)
        {
            if (found >= 0)
                return -2;
            found = value;
        }
    }
    return found;
}
static bool IsText(const UmiLanguageRuntimeJsonDocument *doc, int object, const char *name, const char *value)
{
    int token = Field(doc, object, name);
    char text[64];
    return token >= 0 && UmiLanguageRuntimeJsonText(doc, token, text, sizeof(text)) == UMI_STATUS_OK &&
           strcmp(text, value) == 0;
}
static bool Empty(const UmiLanguageRuntimeJsonDocument *doc, int object, const char *name, bool array)
{
    int token = Field(doc, object, name);
    return token == -1 ||
           (token >= 0 && (umi_language_runtime_json_is_null(doc, token) ||
                           (array && doc->tokens[token].type == UMI_LANGUAGE_RUNTIME_JSON_ARRAY &&
                            umi_language_runtime_json_array_count(doc, token) == 0U)));
}
/* Accumulate text in a temporary result. An invalid later output item must
 * discard earlier text rather than presenting an incomplete successful reply. */
static UmiStatus Append(const UmiLanguageRuntimeJsonDocument *doc, int token, UmiProviderChatResult *result)
{
    if (token < 0 || doc->tokens[token].type != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        return UMI_STATUS_PARSE_ERROR;
    size_t used = strlen(result->text);
    if (used != 0U)
    {
        if (used + 2U >= sizeof(result->text))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        result->text[used++] = '\n';
        result->text[used++] = '\n';
        result->text[used] = '\0';
    }
    return UmiLanguageRuntimeJsonText(doc, token, result->text + used, sizeof(result->text) - used);
}
static UmiStatus Chat(const UmiLanguageRuntimeJsonDocument *doc, UmiProviderChatResult *result)
{
    if (!IsText(doc, 0, "object", "chat.completion"))
        return UMI_STATUS_PARSE_ERROR;
    int choices = Field(doc, 0, "choices");
    if (choices < 0 || doc->tokens[choices].type != UMI_LANGUAGE_RUNTIME_JSON_ARRAY ||
        umi_language_runtime_json_array_count(doc, choices) != 1U)
        return UMI_STATUS_PARSE_ERROR;
    int choice = umi_language_runtime_json_array_at(doc, choices, 0U),
        message = Field(doc, choice, "message");
    if (!IsText(doc, message, "role", "assistant") || !Empty(doc, message, "tool_calls", true) ||
        !Empty(doc, message, "function_call", false))
        return UMI_STATUS_PERMISSION_DENIED;
    result->truncated = IsText(doc, choice, "finish_reason", "length");
    bool filtered = IsText(doc, choice, "finish_reason", "content_filter");
    if (!result->truncated && !filtered && !IsText(doc, choice, "finish_reason", "stop"))
        return UMI_STATUS_PERMISSION_DENIED;
    if (!Empty(doc, message, "refusal", false))
    {
        result->refused = true;
        return Append(doc, Field(doc, message, "refusal"), result);
    }
    if (filtered)
    {
        result->refused = true;
        strcpy(result->text, "The provider filtered this response.");
        return UMI_STATUS_OK;
    }
    return Append(doc, Field(doc, message, "content"), result);
}
static UmiStatus Responses(const UmiLanguageRuntimeJsonDocument *doc, UmiProviderChatResult *result)
{
    if (!IsText(doc, 0, "object", "response"))
        return UMI_STATUS_PARSE_ERROR;
    result->truncated = IsText(doc, 0, "status", "incomplete");
    if (result->truncated)
    {
        int detail = Field(doc, 0, "incomplete_details");
        if (!IsText(doc, detail, "reason", "max_output_tokens"))
            return UMI_STATUS_PERMISSION_DENIED;
    }
    else if (!IsText(doc, 0, "status", "completed"))
        return UMI_STATUS_PARSE_ERROR;
    int output = Field(doc, 0, "output");
    if (output < 0 || doc->tokens[output].type != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        return UMI_STATUS_PARSE_ERROR;
    size_t count = umi_language_runtime_json_array_count(doc, output);
    for (size_t i = 0U; i < count; ++i)
    {
        int item = umi_language_runtime_json_array_at(doc, output, i);
        if (IsText(doc, item, "type", "reasoning"))
            continue;
        if (!IsText(doc, item, "type", "message") || !IsText(doc, item, "role", "assistant"))
            return UMI_STATUS_PERMISSION_DENIED;
        if (!IsText(doc, item, "status", "completed") &&
            !(result->truncated && IsText(doc, item, "status", "incomplete")))
            return UMI_STATUS_PARSE_ERROR;
        int content = Field(doc, item, "content");
        if (content < 0 || doc->tokens[content].type != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            return UMI_STATUS_PARSE_ERROR;
        size_t parts = umi_language_runtime_json_array_count(doc, content);
        for (size_t j = 0U; j < parts; ++j)
        {
            int part = umi_language_runtime_json_array_at(doc, content, j);
            bool refusal = IsText(doc, part, "type", "refusal");
            if (!refusal && !IsText(doc, part, "type", "output_text"))
                return UMI_STATUS_PERMISSION_DENIED;
            UmiStatus status = Append(doc, Field(doc, part, refusal ? "refusal" : "text"), result);
            if (status != UMI_STATUS_OK)
                return status;
            result->refused = result->refused || refusal;
        }
    }
    if (result->text[0] == '\0' && result->truncated)
        strcpy(result->text, "The output limit was reached before visible text was produced.");
    return result->text[0] != '\0' ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}
UmiStatus UmiProviderChatDecode(bool responses, const char *body, size_t length, UmiProviderChatResult *out)
{
    if (body == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length == 0U || length >= UMI_CONNECTION_CHECK_BODY_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(body, '\0', length) != NULL)
        return UMI_STATUS_PARSE_ERROR;
    char *text = malloc(length + 1U);
    UmiLanguageRuntimeJsonDocument *doc = malloc(sizeof(*doc));
    UmiProviderChatResult *result = calloc(1U, sizeof(*result));
    if (text == NULL || doc == NULL || result == NULL)
    {
        free(text);
        free(doc);
        free(result);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(text, body, length);
    text[length] = '\0';
    UmiStatus status = UmiLanguageRuntimeJsonParseComplete(text, doc);
    if (status == UMI_STATUS_OK && !Empty(doc, 0, "error", false))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
    {
        int model = Field(doc, 0, "model");
        status = model < 0 || doc->tokens[model].type != UMI_LANGUAGE_RUNTIME_JSON_STRING
                     ? UMI_STATUS_PARSE_ERROR
                     : UmiLanguageRuntimeJsonText(doc, model, result->model, sizeof(result->model));
    }
    if (status == UMI_STATUS_OK)
        status = responses ? Responses(doc, result) : Chat(doc, result);
    if (status == UMI_STATUS_OK && (!TextValid(result->text, sizeof(result->text), true) ||
                                    !TextValid(result->model, sizeof(result->model), true)))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        *out = *result;
    umi_secret_clear(text, length + 1U);
    umi_secret_clear(result, sizeof(*result));
    free(text);
    free(doc);
    free(result);
    return status;
}
