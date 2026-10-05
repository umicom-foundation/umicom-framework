/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/adapter_preferences.c
 * PURPOSE: Keep native adapter preferences in bounded user-local records shared by product hosts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/adapter_preferences.h"
#include "umicom/document/text_encoding.h"
#include "umicom/language_runtime/json_tree.h"
#include "umicom/language_runtime/json_writer.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/local_replace.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/resource_location.h"
#include <stdlib.h>
#include <string.h>
UmiStatus UmiDebugAdapterPreferencesValidate(const UmiDebugAdapterPreferences *preferences)
{
    if (preferences == NULL || memchr(preferences->kind, 0, sizeof(preferences->kind)) == NULL ||
        memchr(preferences->executable, 0, sizeof(preferences->executable)) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(preferences->kind, "gdb") != 0 && strcmp(preferences->kind, "lldb") != 0)
        return UMI_STATUS_NOT_IMPLEMENTED;
    size_t length = strlen(preferences->executable);
    if (!umi_document_utf8_validate((const unsigned char *)preferences->executable, length, NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < length; ++i)
        if ((unsigned char)preferences->executable[i] < 0x20U ||
            (unsigned char)preferences->executable[i] == 0x7fU)
            return UMI_STATUS_INVALID_ARGUMENT;
    return length == 0U ? UMI_STATUS_OK : UmiOutputFileValidatePath(preferences->executable);
}
UmiStatus UmiDebugAdapterPreferencesEncode(const UmiDebugAdapterPreferences *preferences, char *out,
                                           size_t capacity, size_t *outSize)
{
    if (out == NULL || outSize == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiDebugAdapterPreferencesValidate(preferences);
    if (status != UMI_STATUS_OK)
        return status;
    char encoded[8192];
    UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, encoded, sizeof(encoded));
    (void)umi_language_runtime_json_writer_raw(&writer,
                                               "{\"format\":\"umicom.native-debug-adapter\",\"adapter\":");
    (void)umi_language_runtime_json_writer_string(&writer, preferences->kind);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"executable\":");
    (void)umi_language_runtime_json_writer_string(&writer, preferences->executable);
    (void)umi_language_runtime_json_writer_raw(&writer, "}\n");
    if (writer.status != UMI_STATUS_OK)
        return writer.status;
    if (writer.length >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, encoded, writer.length + 1U);
    *outSize = writer.length;
    return UMI_STATUS_OK;
}
UmiStatus UmiDebugAdapterPreferencesDecode(const void *bytes, size_t size, UmiDebugAdapterPreferences *out)
{
    if (bytes == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Restrict this format to its reviewed fields. Silently accepting future
     * execution options would imply capabilities this preferences UI lacks. */
    UmiJsonTreeLimits limits = {8192U, 32U, 4U};
    UmiJsonTree *tree = NULL;
    UmiStatus status = UmiJsonTreeCreate(bytes, size, &limits, NULL, &tree);
    UmiDebugAdapterPreferences candidate = {0};
    if (status == UMI_STATUS_OK &&
        (UmiJsonTreeKind(tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT || UmiJsonTreeCount(tree, 0) != 3U))
        status = UMI_STATUS_PARSE_ERROR;
    const char *fields[] = {"format", "adapter", "executable"};
    char format[64] = {0};
    char *destinations[] = {format, candidate.kind, candidate.executable};
    size_t capacities[] = {sizeof(format), sizeof(candidate.kind), sizeof(candidate.executable)};
    for (size_t i = 0U; status == UMI_STATUS_OK && i < 3U; ++i)
    {
        int node = -1;
        status = UmiJsonTreeMember(tree, 0, fields[i], &node);
        if (status == UMI_STATUS_OK)
            status = UmiJsonTreeText(tree, node, destinations[i], capacities[i]);
    }
    if (status == UMI_STATUS_OK && strcmp(format, "umicom.native-debug-adapter") != 0)
        status = UMI_STATUS_NOT_IMPLEMENTED;
    if (status == UMI_STATUS_OK)
        status = UmiDebugAdapterPreferencesValidate(&candidate);
    UmiJsonTreeDestroy(tree);
    if (status == UMI_STATUS_OK)
        *out = candidate;
    return status;
}
UmiStatus UmiDebugAdapterPreferencesPath(const char *applicationDirectory, const char *baseOverride,
                                         char *out, size_t capacity)
{
    if (out == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiApplicationPathsConfig config = UmiApplicationPathsConfigDefault(applicationDirectory);
    config.baseOverride = baseOverride;
    /* The path bundle is sizeable; keep it off small callback thread stacks. */
    UmiApplicationPaths *paths = calloc(1U, sizeof(*paths));
    if (paths == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiApplicationPathsResolve(&config, paths);
    char path[UMI_PATH_CAPACITY];
    if (status == UMI_STATUS_OK)
        status = umi_path_join(paths->config, "native-debug-adapter.json", path, sizeof(path));
    if (status == UMI_STATUS_OK)
        status = UmiOutputFileValidatePath(path);
    if (status == UMI_STATUS_OK)
    {
        size_t length = strlen(path);
        if (length >= capacity)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else
            memcpy(out, path, length + 1U);
    }
    free(paths);
    return status;
}
UmiStatus UmiDebugAdapterPreferencesLoad(const char *path, UmiDebugAdapterPreferences *out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    UmiStatus status = UmiInputFileRead(path, 8192U, &bytes, &size);
    if (status == UMI_STATUS_OK)
        status = UmiDebugAdapterPreferencesDecode(bytes, size, out);
    UmiInputFileFree(bytes);
    return status;
}
UmiStatus UmiDebugAdapterPreferencesSave(const char *path, const UmiDebugAdapterPreferences *preferences)
{
    char encoded[8192];
    size_t size;
    UmiStatus status = UmiDebugAdapterPreferencesEncode(preferences, encoded, sizeof(encoded), &size);
    return status == UMI_STATUS_OK ? UmiLocalFileReplace(path, encoded, size) : status;
}
