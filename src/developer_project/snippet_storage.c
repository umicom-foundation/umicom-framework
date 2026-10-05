/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/snippet_storage.c
 * PURPOSE: Store bounded named templates as local data while reusing native path and atomic file ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_project/snippet_storage.h"
#include "umicom/document/text_encoding.h"
#include "umicom/language_runtime/json_tree.h"
#include "umicom/language_runtime/json_writer.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/local_replace.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/resource_location.h"
#include <stdlib.h>
#include <string.h>

static UmiStatus SnippetStoredText(const char *text, size_t capacity, int body)
{
    if (text == NULL || memchr(text, '\0', capacity) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t bytes = strlen(text);
    if (!umi_document_utf8_validate((const unsigned char *)text, bytes, NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    int visible = 0;
    for (size_t i = 0U; i < bytes; ++i)
    {
        unsigned char ch = (unsigned char)text[i];
        if (!body && (ch < 0x20U || ch == 0x7fU))
            return UMI_STATUS_INVALID_ARGUMENT;
        if (ch > 0x20U)
            visible = 1;
    }
    return !body && !visible ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK;
}
static UmiStatus SnippetStoredIdentity(const char *text, size_t capacity)
{
    if (text == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t bytes = strlen(text);
    return bytes >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : SnippetStoredText(text, bytes + 1U, 0);
}
UmiStatus UmiSnippetTemplateValidate(const UmiEditorSnippetTemplate *snippet)
{
    if (snippet == NULL || snippet->struct_size != (uint32_t)sizeof(*snippet) ||
        snippet->api_version != UMI_EDITOR_SNIPPET_SESSION_API_VERSION)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = SnippetStoredText(snippet->id, sizeof(snippet->id), 0);
    if (status == UMI_STATUS_OK)
        status = SnippetStoredText(snippet->language_id, sizeof(snippet->language_id), 0);
    if (status == UMI_STATUS_OK)
        status = SnippetStoredText(snippet->name, sizeof(snippet->name), 0);
    if (status == UMI_STATUS_OK)
        status = SnippetStoredText(snippet->body, sizeof(snippet->body), 1);
    return status;
}
UmiStatus UmiSnippetTemplateEncode(const UmiEditorSnippetTemplate *snippet, char *out, size_t capacity,
                                   size_t *out_bytes)
{
    if (out == NULL || out_bytes == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiSnippetTemplateValidate(snippet);
    if (status != UMI_STATUS_OK)
        return status;
    /* Every input byte can require up to six JSON bytes. The fixed input
     * capacities fit this bound, including all keys and the final terminator. */
    char *encoded = malloc(65536U);
    if (encoded == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, encoded, 65536U);
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"format\":\"umicom.local-snippet\",\"id\":");
    (void)umi_language_runtime_json_writer_string(&writer, snippet->id);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"language\":");
    (void)umi_language_runtime_json_writer_string(&writer, snippet->language_id);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"name\":");
    (void)umi_language_runtime_json_writer_string(&writer, snippet->name);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"body\":");
    (void)umi_language_runtime_json_writer_string(&writer, snippet->body);
    (void)umi_language_runtime_json_writer_raw(&writer, "}\n");
    status = writer.status;
    if (status == UMI_STATUS_OK && writer.length >= capacity)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
    {
        memcpy(out, encoded, writer.length + 1U);
        *out_bytes = writer.length;
    }
    free(encoded);
    return status;
}
UmiStatus UmiSnippetTemplateDecode(const void *bytes, size_t size, UmiEditorSnippetTemplate *out)
{
    if (bytes == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {65536U, 32U, 4U};
    UmiStatus status = UmiJsonTreeCreate(bytes, size, &limits, NULL, &tree);
    if (status == UMI_STATUS_OK &&
        (UmiJsonTreeKind(tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT || UmiJsonTreeCount(tree, 0) != 5U))
        status = UMI_STATUS_PARSE_ERROR;
    UmiEditorSnippetTemplate *candidate = calloc(1U, sizeof(*candidate));
    if (candidate == NULL)
    {
        UmiJsonTreeDestroy(tree);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    candidate->struct_size = (uint32_t)sizeof(*candidate);
    candidate->api_version = UMI_EDITOR_SNIPPET_SESSION_API_VERSION;
    char format[64];
    const char *fields[] = {"format", "id", "language", "name", "body"};
    char *values[] = {format, candidate->id, candidate->language_id, candidate->name, candidate->body};
    size_t capacities[] = {sizeof(format), sizeof(candidate->id), sizeof(candidate->language_id),
                           sizeof(candidate->name), sizeof(candidate->body)};
    for (size_t i = 0U; status == UMI_STATUS_OK && i < 5U; ++i)
    {
        int node = -1;
        status = UmiJsonTreeMember(tree, 0, fields[i], &node);
        if (status == UMI_STATUS_OK)
            status = UmiJsonTreeText(tree, node, values[i], capacities[i]);
    }
    if (status == UMI_STATUS_OK && strcmp(format, "umicom.local-snippet") != 0)
        status = UMI_STATUS_NOT_IMPLEMENTED;
    if (status == UMI_STATUS_OK)
        status = UmiSnippetTemplateValidate(candidate);
    if (status == UMI_STATUS_OK)
        *out = *candidate;
    free(candidate);
    UmiJsonTreeDestroy(tree);
    return status;
}
UmiStatus UmiSnippetTemplatePath(const char *application_directory, const char *base_override,
                                 const char *language_id, const char *template_id, char *out, size_t capacity)
{
    if (out == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = SnippetStoredIdentity(language_id, UMI_EDITOR_SNIPPET_LANGUAGE_CAPACITY);
    if (status == UMI_STATUS_OK)
        status = SnippetStoredIdentity(template_id, UMI_EDITOR_SNIPPET_ID_CAPACITY);
    if (status != UMI_STATUS_OK)
        return status;
    /* Keep each encoded identity in its own component. Even the maximum
     * template ID then stays below common 255-byte filesystem name limits. */
    char language_hex[UMI_EDITOR_SNIPPET_LANGUAGE_CAPACITY * 2U];
    char id_hex[UMI_EDITOR_SNIPPET_ID_CAPACITY * 2U];
    char *encoded[] = {language_hex, id_hex};
    const char *identities[] = {language_id, template_id};
    static const char hex[] = "0123456789abcdef";
    for (size_t i = 0U; i < 2U; ++i)
    {
        size_t used = 0U;
        for (const unsigned char *p = (const unsigned char *)identities[i]; *p != '\0'; ++p)
        {
            encoded[i][used++] = hex[*p >> 4U];
            encoded[i][used++] = hex[*p & 15U];
        }
        encoded[i][used] = '\0';
    }
    UmiApplicationPaths *paths = calloc(1U, sizeof(*paths));
    if (paths == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiApplicationPathsConfig config = UmiApplicationPathsConfigDefault(application_directory);
    config.baseOverride = base_override;
    status = UmiApplicationPathsResolve(&config, paths);
    char root[UMI_PATH_CAPACITY], language_path[UMI_PATH_CAPACITY], id_path[UMI_PATH_CAPACITY],
        path[UMI_PATH_CAPACITY];
    if (status == UMI_STATUS_OK)
        status = umi_path_join(paths->config, "snippets", root, sizeof(root));
    if (status == UMI_STATUS_OK)
        status = umi_path_join(root, language_hex, language_path, sizeof(language_path));
    if (status == UMI_STATUS_OK)
        status = umi_path_join(language_path, id_hex, id_path, sizeof(id_path));
    if (status == UMI_STATUS_OK)
        status = umi_path_join(id_path, "template.json", path, sizeof(path));
    if (status == UMI_STATUS_OK)
        status = UmiOutputFileValidatePath(path);
    if (status == UMI_STATUS_OK && strlen(path) >= capacity)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
        memcpy(out, path, strlen(path) + 1U);
    free(paths);
    return status;
}
UmiStatus UmiSnippetTemplateLoad(const char *path, const char *expected_language, const char *expected_id,
                                 UmiEditorSnippetTemplate *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = SnippetStoredIdentity(expected_language, UMI_EDITOR_SNIPPET_LANGUAGE_CAPACITY);
    if (status == UMI_STATUS_OK)
        status = SnippetStoredIdentity(expected_id, UMI_EDITOR_SNIPPET_ID_CAPACITY);
    if (status != UMI_STATUS_OK)
        return status;
    UmiEditorSnippetTemplate *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    status = UmiInputFileRead(path, 65536U, &bytes, &size);
    if (status == UMI_STATUS_OK)
        status = UmiSnippetTemplateDecode(bytes, size, candidate);
    if (status == UMI_STATUS_OK &&
        (strcmp(candidate->language_id, expected_language) != 0 || strcmp(candidate->id, expected_id) != 0))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        *out = *candidate;
    UmiInputFileFree(bytes);
    free(candidate);
    return status;
}
UmiStatus UmiSnippetTemplateSave(const char *path, const UmiEditorSnippetTemplate *snippet)
{
    char *bytes = malloc(65536U);
    if (bytes == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    size_t size = 0U;
    UmiStatus status = UmiSnippetTemplateEncode(snippet, bytes, 65536U, &size);
    if (status == UMI_STATUS_OK)
        status = UmiLocalFileReplace(path, bytes, size);
    free(bytes);
    return status;
}
