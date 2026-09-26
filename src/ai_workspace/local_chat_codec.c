/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/local_chat_codec.c
 *
 * PURPOSE:
 *   Encode bounded chat requests and reject malformed, oversized or action-bearing model replies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "local_chat_internal.h"
#include "workspace_internal.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#ifdef UMICOM_AI_WORKSPACE_HAS_HTTP
#include <json-c/json.h>

static UmiStatus AwJsonString(json_object *object, const char *name, char *out, size_t capacity, bool optional)
{
    json_object *value = NULL;
    if (!json_object_object_get_ex(object, name, &value)) return optional ? UMI_STATUS_NOT_FOUND : UMI_STATUS_PARSE_ERROR;
    if (!json_object_is_type(value, json_type_string)) return UMI_STATUS_PARSE_ERROR;
    const char *text = json_object_get_string(value); int length = json_object_get_string_len(value);
    if (length < 0 || (size_t)length >= capacity || strlen(text) != (size_t)length) return UMI_STATUS_PARSE_ERROR;
    return AwTextCopy(out, capacity, text, false) == UMI_STATUS_OK ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}
static UmiStatus AwJsonNumber(json_object *object, const char *name, uint64_t *out)
{
    json_object *value = NULL;
    if (!json_object_object_get_ex(object, name, &value) || !json_object_is_type(value, json_type_int)) return UMI_STATUS_PARSE_ERROR;
    int64_t signedValue = json_object_get_int64(value);
    if (signedValue < 0 || json_object_get_uint64(value) > INT64_MAX) return UMI_STATUS_PARSE_ERROR;
    *out = (uint64_t)signedValue; return UMI_STATUS_OK;
}
static int AwJsonAdd(json_object *object, const char *key, json_object *value)
{
    if (value == NULL) return -1;
    if (json_object_object_add(object, key, value) != 0) { json_object_put(value); return -1; }
    return 0;
}
UmiStatus AwLocalChatEncode(const UmiAiRequest *request, char **outJson)
{
    json_object *root = NULL, *messages = NULL; UmiStatus status = UMI_STATUS_OUT_OF_MEMORY;
    if (outJson == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outJson = NULL;
    if (request == NULL || request->message_count == 0U || request->message_count > UMI_AI_MAX_MESSAGES ||
        !AwTextValid(request->request_id, sizeof(request->request_id), false) ||
        !AwTextValid(request->model_id, sizeof(request->model_id), false) || request->allow_tools ||
        request->max_output_tokens == 0U || request->max_output_tokens > 4096U ||
        !isfinite(request->temperature) || request->temperature < 0.0 || request->temperature > 2.0) return UMI_STATUS_INVALID_ARGUMENT;
    root = json_object_new_object(); messages = json_object_new_array();
    if (root == NULL || messages == NULL) goto cleanup;
    if (json_object_object_add(root, "messages", messages) != 0) goto cleanup;
    messages = NULL; /* Root owns the array from this point. */
    json_object *array = NULL; (void)json_object_object_get_ex(root, "messages", &array);
    if (AwJsonAdd(root, "model", json_object_new_string(request->model_id)) != 0 ||
        AwJsonAdd(root, "max_tokens", json_object_new_int64((int64_t)request->max_output_tokens)) != 0 ||
        AwJsonAdd(root, "temperature", json_object_new_double(request->temperature)) != 0 ||
        AwJsonAdd(root, "stream", json_object_new_boolean(0)) != 0) goto cleanup;
    for (size_t i = 0U; i < request->message_count; ++i) {
        const UmiAiMessage *message = &request->messages[i]; const char *role;
        if (!AwTextValid(message->text, sizeof(message->text), false)) { status = UMI_STATUS_INVALID_ARGUMENT; goto cleanup; }
        switch (message->role) {
        case UMI_AI_ROLE_SYSTEM: role = "system"; break;
        case UMI_AI_ROLE_USER: role = "user"; break;
        case UMI_AI_ROLE_ASSISTANT: role = "assistant"; break;
        default: status = UMI_STATUS_PERMISSION_DENIED; goto cleanup;
        }
        json_object *entry = json_object_new_object();
        if (entry == NULL) goto cleanup;
        if (AwJsonAdd(entry, "role", json_object_new_string(role)) != 0 ||
            AwJsonAdd(entry, "content", json_object_new_string(message->text)) != 0 || json_object_array_add(array, entry) != 0) {
            json_object_put(entry); goto cleanup;
        }
    }
    const char *encoded = json_object_to_json_string_ext(root, JSON_C_TO_STRING_PLAIN);
    if (encoded == NULL) goto cleanup;
    size_t length = strlen(encoded);
    if (length >= AW_HTTP_BODY_CAPACITY) { status = UMI_STATUS_CAPACITY_EXCEEDED; goto cleanup; }
    *outJson = malloc(length + 1U); if (*outJson == NULL) goto cleanup;
    memcpy(*outJson, encoded, length + 1U); status = UMI_STATUS_OK;
cleanup:
    if (messages != NULL) json_object_put(messages);
    if (root != NULL) json_object_put(root);
    return status;
}
/* json-c 0.18 can reject valid non-ASCII text when STRICT and UTF8
 * validation are combined (upstream issue 892). Keep strict parsing enabled:
 * validate UTF-8 ourselves, then represent non-ASCII scalars as equivalent
 * JSON Unicode escapes. This does not accept comments, malformed JSON or
 * invalid UTF-8 and works without altering the user's decoded text. */
static char *AwAsciiJson(const char *json, size_t length, size_t *outLength)
{
    char *copy = malloc(length + 1U);
    if (copy == NULL) return NULL;
    memcpy(copy, json, length); copy[length] = '\0';
    if (!AwTextValid(copy, length + 1U, false)) { free(copy); return NULL; }
    char *ascii = malloc(length * 6U + 1U); size_t written = 0U;
    if (ascii == NULL) { free(copy); return NULL; }
    static const char HEX[] = "0123456789abcdef";
    for (size_t i = 0U; i < length;) {
        unsigned char first = (unsigned char)copy[i++];
        if (first < 0x80U) { ascii[written++] = (char)first; continue; }
        size_t count = first < 0xE0U ? 1U : first < 0xF0U ? 2U : 3U;
        uint32_t code = (uint32_t)(first & (count == 1U ? 0x1FU : count == 2U ? 0x0FU : 0x07U));
        for (size_t n = 0U; n < count; ++n) code = (code << 6U) | ((uint32_t)(unsigned char)copy[i++] & 0x3FU);
        uint32_t units[2]; size_t unitCount = 1U;
        units[0] = code;
        if (code > 0xFFFFU) { code -= 0x10000U; units[0] = 0xD800U + (code >> 10U); units[1] = 0xDC00U + (code & 0x3FFU); unitCount = 2U; }
        for (size_t u = 0U; u < unitCount; ++u) {
            ascii[written++] = '\\'; ascii[written++] = 'u';
            for (unsigned shift = 16U; shift > 0U; shift -= 4U) ascii[written++] = HEX[(units[u] >> (shift - 4U)) & 15U];
        }
    }
    ascii[written] = '\0'; *outLength = written; free(copy); return ascii;
}
UmiStatus AwLocalChatDecode(const UmiAiRequest *request, const char *json, size_t length, UmiAiResponse *response)
{
    json_tokener *parser; char *ascii = NULL; size_t asciiLength = 0U; json_object *root = NULL, *choices = NULL, *choice = NULL, *message = NULL, *value = NULL;
    UmiStatus status = UMI_STATUS_PARSE_ERROR; UmiAiResponse result = {0}; char role[32], finish[32];
    if (request == NULL || json == NULL || response == NULL || length == 0U || length >= AW_HTTP_BODY_CAPACITY ||
        !AwTextValid(request->request_id, sizeof(request->request_id), false) ||
        !AwTextValid(request->model_id, sizeof(request->model_id), false)) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(json, '\0', length) != NULL) return UMI_STATUS_PARSE_ERROR;
    parser = json_tokener_new_ex(24); if (parser == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    json_tokener_set_flags(parser, JSON_TOKENER_STRICT | JSON_TOKENER_VALIDATE_UTF8);
    ascii = AwAsciiJson(json, length, &asciiLength);
    if (ascii == NULL) goto cleanup;
    root = json_tokener_parse_ex(parser, ascii, (int)asciiLength);
    if (json_tokener_get_error(parser) != json_tokener_success || !json_object_is_type(root, json_type_object)) goto cleanup;
    size_t parsed = json_tokener_get_parse_end(parser);
    for (size_t i = parsed; i < asciiLength; ++i)
        if (ascii[i] != ' ' && ascii[i] != '\n' && ascii[i] != '\r' && ascii[i] != '\t') goto cleanup;
    if (json_object_object_get_ex(root, "error", &value) ||
        !json_object_object_get_ex(root, "choices", &choices) || !json_object_is_type(choices, json_type_array) ||
        json_object_array_length(choices) != 1U) goto cleanup;
    choice = json_object_array_get_idx(choices, 0U);
    if (!json_object_is_type(choice, json_type_object) ||
        !json_object_object_get_ex(choice, "message", &message) || !json_object_is_type(message, json_type_object)) goto cleanup;
    if (json_object_object_get_ex(message, "tool_calls", &value) || json_object_object_get_ex(message, "function_call", &value)) {
        status = UMI_STATUS_PERMISSION_DENIED; goto cleanup;
    }
    if (AwJsonString(message, "role", role, sizeof(role), false) != UMI_STATUS_OK || strcmp(role, "assistant") != 0 ||
        AwJsonString(message, "content", result.text, sizeof(result.text), false) != UMI_STATUS_OK ||
        AwJsonString(choice, "finish_reason", finish, sizeof(finish), false) != UMI_STATUS_OK) goto cleanup;
    if (strcmp(finish, "stop") == 0) result.finish_reason = UMI_AI_FINISH_STOP;
    else if (strcmp(finish, "length") == 0) result.finish_reason = UMI_AI_FINISH_LENGTH;
    else goto cleanup;
    /* A reported model must match the model that was reviewed. This avoids
     * quietly attaching an unreviewed model identity to a saved response. */
    UmiStatus modelStatus = AwJsonString(root, "model", result.model_id, sizeof(result.model_id), true);
    if (modelStatus == UMI_STATUS_NOT_FOUND) (void)AwTextCopy(result.model_id, sizeof(result.model_id), request->model_id, false);
    else if (modelStatus != UMI_STATUS_OK || strcmp(result.model_id, request->model_id) != 0) goto cleanup;
    if (json_object_object_get_ex(root, "usage", &value) && value != NULL) {
        if (!json_object_is_type(value, json_type_object) ||
            AwJsonNumber(value, "prompt_tokens", &result.usage.input_tokens) != UMI_STATUS_OK ||
            AwJsonNumber(value, "completion_tokens", &result.usage.output_tokens) != UMI_STATUS_OK ||
            AwJsonNumber(value, "total_tokens", &result.usage.total_tokens) != UMI_STATUS_OK ||
            result.usage.total_tokens != result.usage.input_tokens + result.usage.output_tokens) goto cleanup;
    }
    (void)AwTextCopy(result.request_id, sizeof(result.request_id), request->request_id, false);
    (void)AwTextCopy(result.provider_id, sizeof(result.provider_id), "umicom.local-chat", false);
    *response = result; status = UMI_STATUS_OK;
cleanup:
    if (root != NULL) json_object_put(root);
    free(ascii); json_tokener_free(parser); return status;
}
#else
UmiStatus AwLocalChatEncode(const UmiAiRequest *request, char **outJson)
{ (void)request; if (outJson != NULL) *outJson = NULL; return UMI_STATUS_UNAVAILABLE; }
UmiStatus AwLocalChatDecode(const UmiAiRequest *request, const char *json, size_t length, UmiAiResponse *response)
{ (void)request; (void)json; (void)length; (void)response; return UMI_STATUS_UNAVAILABLE; }
#endif
void AwLocalChatFree(char *json) { free(json); }
