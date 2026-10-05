/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/provider_connection_checks/catalogue.c
 * PURPOSE: Read bounded model catalogues using Framework complete JSON validation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/language_runtime/json_document.h"
#include "umicom/language_runtime/json_text.h"
#include <stdlib.h>
#include <string.h>

static int Field(const UmiLanguageRuntimeJsonDocument *doc, int object, const char *name)
{
    int found = -1;
    size_t count = umi_language_runtime_json_object_count(doc, object);
    for (size_t i = 0U; i < count; ++i) {
        int key, value; char text[32];
        if (umi_language_runtime_json_object_entry_at(doc, object, i, &key, &value) != UMI_STATUS_OK) return -1;
        UmiStatus status = UmiLanguageRuntimeJsonText(doc, key, text, sizeof(text));
        /* Long extension names cannot equal one of our short required names.
         * Complete parsing has already validated their contents. */
        if (status == UMI_STATUS_CAPACITY_EXCEEDED) continue;
        if (status != UMI_STATUS_OK) return -1;
        if (strcmp(text, name) == 0) { if (found >= 0) return -1; found = value; }
    }
    return found;
}
static bool TextEquals(const UmiLanguageRuntimeJsonDocument *doc, int token, const char *expected)
{
    char text[32];
    return token >= 0 && UmiLanguageRuntimeJsonText(doc, token, text, sizeof(text)) == UMI_STATUS_OK && strcmp(text, expected) == 0;
}
UmiStatus UmiConnectionCheckDecode(const char *body, size_t length, const char *model,
    UmiProviderConnectionCheckResult *out)
{
    if (body == NULL || model == NULL || out == NULL || model[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (length == 0U || length >= UMI_CONNECTION_CHECK_BODY_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(body, '\0', length) != NULL) return UMI_STATUS_PARSE_ERROR;
    char *text = malloc(length + 1U);
    UmiLanguageRuntimeJsonDocument *doc = malloc(sizeof(*doc));
    if (text == NULL || doc == NULL) { free(text); free(doc); return UMI_STATUS_OUT_OF_MEMORY; }
    memcpy(text, body, length); text[length] = '\0';
    UmiStatus status = UmiLanguageRuntimeJsonParseComplete(text, doc);
    UmiProviderConnectionCheckResult result = {0};
    if (status == UMI_STATUS_OK) {
        int data = Field(doc, 0, "data");
        if (doc->token_count == 0U || doc->tokens[0].type != UMI_LANGUAGE_RUNTIME_JSON_OBJECT ||
            !TextEquals(doc, Field(doc, 0, "object"), "list") || data < 0 ||
            doc->tokens[data].type != UMI_LANGUAGE_RUNTIME_JSON_ARRAY) status = UMI_STATUS_PARSE_ERROR;
        else {
            result.listed_models = umi_language_runtime_json_array_count(doc, data);
            for (size_t i = 0U; status == UMI_STATUS_OK && i < result.listed_models; ++i) {
                int item = umi_language_runtime_json_array_at(doc, data, i);
                char id[513];
                if (item < 0 || doc->tokens[item].type != UMI_LANGUAGE_RUNTIME_JSON_OBJECT ||
                    !TextEquals(doc, Field(doc, item, "object"), "model")) { status = UMI_STATUS_PARSE_ERROR; break; }
                int id_token = Field(doc, item, "id");
                if (id_token < 0 || doc->tokens[id_token].type != UMI_LANGUAGE_RUNTIME_JSON_STRING) {
                    status = UMI_STATUS_PARSE_ERROR; break;
                }
                status = UmiLanguageRuntimeJsonText(doc, id_token, id, sizeof(id));
                if (status == UMI_STATUS_OK && id[0] == '\0') status = UMI_STATUS_PARSE_ERROR;
                if (status == UMI_STATUS_OK && strcmp(id, model) == 0) result.model_listed = true;
            }
        }
    }
    /* No raw service text escapes this decoder. A valid list without the
     * chosen model is distinct from an unreadable or truncated response. */
    if (status == UMI_STATUS_OK) { *out = result; if (!result.model_listed) status = UMI_STATUS_NOT_FOUND; }
    umi_secret_clear(text, length + 1U); free(text); free(doc); return status;
}
