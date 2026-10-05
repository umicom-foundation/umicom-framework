/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/server_preferences.c
 * PURPOSE: Share strict local language-server settings while keeping loading separate from execution.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/server_preferences.h"
#include "umicom/base/arguments.h"
#include "umicom/document/text_encoding.h"
#include "umicom/language_runtime/json_tree.h"
#include "umicom/language_runtime/json_writer.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/local_replace.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/resource_location.h"
#include <stdlib.h>
#include <string.h>

static UmiStatus ServerPreferenceText(const char *text, size_t capacity, int empty)
{
    if (text == NULL || memchr(text, '\0', capacity) == NULL || (!empty && text[0] == '\0'))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t bytes = strlen(text);
    if (!umi_document_utf8_validate((const unsigned char *)text, bytes, NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < bytes; ++i)
        if ((unsigned char)text[i] < 0x20U || (unsigned char)text[i] == 0x7fU)
            return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
static UmiStatus ServerPreferenceLanguage(const char *language)
{
    /* External language pointers need only hold their terminated spelling;
     * do not scan a fixed struct-sized range beyond that terminator. */
    if (language == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = strlen(language);
    if (length == 0U || length >= sizeof(((UmiLanguageServerPreferences *)0)->language_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    return ServerPreferenceText(language, length + 1U, 0);
}
UmiStatus UmiLanguageServerPreferencesValidate(const UmiLanguageServerPreferences *preferences)
{
    if (preferences == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = ServerPreferenceText(preferences->language_id, sizeof(preferences->language_id), 0);
    if (status == UMI_STATUS_OK)
        status = ServerPreferenceText(preferences->executable, sizeof(preferences->executable), 0);
    if (status == UMI_STATUS_OK)
        status = ServerPreferenceText(preferences->arguments, sizeof(preferences->arguments), 1);
    if (status == UMI_STATUS_OK)
        status = UmiOutputFileValidatePath(preferences->executable);
    if (status != UMI_STATUS_OK)
        return status;
    /* The argument vector can be sizeable. Keep its owned scratch off a GUI
     * worker's stack, and validate with the same grammar the launcher uses. */
    UmiArguments *parsed = malloc(sizeof(*parsed));
    if (parsed == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiArgumentsParse(preferences->arguments, parsed);
    free(parsed);
    return status;
}
UmiStatus UmiLanguageServerPreferencesEncode(const UmiLanguageServerPreferences *preferences, char *out,
                                             size_t capacity, size_t *out_size)
{
    if (out == NULL || out_size == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiLanguageServerPreferencesValidate(preferences);
    if (status != UMI_STATUS_OK)
        return status;
    char *encoded = malloc(32768U);
    if (encoded == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, encoded, 32768U);
    umi_language_runtime_json_writer_raw(&writer,
                                         "{\"format\":\"umicom.local-language-server\",\"language\":");
    umi_language_runtime_json_writer_string(&writer, preferences->language_id);
    umi_language_runtime_json_writer_raw(&writer, ",\"executable\":");
    umi_language_runtime_json_writer_string(&writer, preferences->executable);
    umi_language_runtime_json_writer_raw(&writer, ",\"arguments\":");
    umi_language_runtime_json_writer_string(&writer, preferences->arguments);
    umi_language_runtime_json_writer_raw(&writer, "}\n");
    status = writer.status;
    if (status == UMI_STATUS_OK && writer.length >= capacity)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
    {
        memcpy(out, encoded, writer.length + 1U);
        *out_size = writer.length;
    }
    free(encoded);
    return status;
}
UmiStatus UmiLanguageServerPreferencesDecode(const void *bytes, size_t size,
                                             UmiLanguageServerPreferences *out)
{
    if (bytes == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {32768U, 32U, 4U};
    UmiStatus status = UmiJsonTreeCreate(bytes, size, &limits, NULL, &tree);
    if (status == UMI_STATUS_OK &&
        (UmiJsonTreeKind(tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT || UmiJsonTreeCount(tree, 0) != 4U))
        status = UMI_STATUS_PARSE_ERROR;
    UmiLanguageServerPreferences candidate = {0};
    char format[64];
    const char *fields[] = {"format", "language", "executable", "arguments"};
    char *values[] = {format, candidate.language_id, candidate.executable, candidate.arguments};
    size_t capacities[] = {sizeof(format), sizeof(candidate.language_id), sizeof(candidate.executable),
                           sizeof(candidate.arguments)};
    for (size_t i = 0U; status == UMI_STATUS_OK && i < 4U; ++i)
    {
        int node = -1;
        status = UmiJsonTreeMember(tree, 0, fields[i], &node);
        if (status == UMI_STATUS_OK)
            status = UmiJsonTreeText(tree, node, values[i], capacities[i]);
    }
    if (status == UMI_STATUS_OK && strcmp(format, "umicom.local-language-server") != 0)
        status = UMI_STATUS_NOT_IMPLEMENTED;
    if (status == UMI_STATUS_OK)
        status = UmiLanguageServerPreferencesValidate(&candidate);
    UmiJsonTreeDestroy(tree);
    if (status == UMI_STATUS_OK)
        *out = candidate;
    return status;
}
UmiStatus UmiLanguageServerPreferencesPath(const char *application_directory, const char *base_override,
                                           const char *language_id, char *out, size_t capacity)
{
    if (out == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = ServerPreferenceLanguage(language_id);
    if (status != UMI_STATUS_OK)
        return status;
    char leaf[160] = "language-server-";
    static const char hex[] = "0123456789abcdef";
    size_t used = strlen(leaf);
    for (const unsigned char *p = (const unsigned char *)language_id; *p != 0U; ++p)
    {
        leaf[used++] = hex[*p >> 4U];
        leaf[used++] = hex[*p & 15U];
    }
    memcpy(leaf + used, ".json", 6U);
    UmiApplicationPaths *paths = calloc(1U, sizeof(*paths));
    if (paths == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiApplicationPathsConfig config = UmiApplicationPathsConfigDefault(application_directory);
    config.baseOverride = base_override;
    status = UmiApplicationPathsResolve(&config, paths);
    char path[UMI_PATH_CAPACITY];
    if (status == UMI_STATUS_OK)
        status = umi_path_join(paths->config, leaf, path, sizeof(path));
    if (status == UMI_STATUS_OK)
        status = UmiOutputFileValidatePath(path);
    if (status == UMI_STATUS_OK && strlen(path) >= capacity)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
        memcpy(out, path, strlen(path) + 1U);
    free(paths);
    return status;
}
UmiStatus UmiLanguageServerPreferencesLoad(const char *path, const char *expected_language_id,
                                           UmiLanguageServerPreferences *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = ServerPreferenceLanguage(expected_language_id);
    if (status != UMI_STATUS_OK)
        return status;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    UmiLanguageServerPreferences candidate;
    status = UmiInputFileRead(path, 32768U, &bytes, &size);
    if (status == UMI_STATUS_OK)
        status = UmiLanguageServerPreferencesDecode(bytes, size, &candidate);
    UmiInputFileFree(bytes);
    if (status == UMI_STATUS_OK && strcmp(candidate.language_id, expected_language_id) != 0)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        *out = candidate;
    return status;
}
UmiStatus UmiLanguageServerPreferencesSave(const char *path, const UmiLanguageServerPreferences *preferences)
{
    char *bytes = malloc(32768U);
    if (bytes == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    size_t size = 0U;
    UmiStatus status = UmiLanguageServerPreferencesEncode(preferences, bytes, 32768U, &size);
    if (status == UMI_STATUS_OK)
        status = UmiLocalFileReplace(path, bytes, size);
    free(bytes);
    return status;
}
