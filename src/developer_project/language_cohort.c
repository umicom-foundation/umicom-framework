/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/language_cohort.c
 *
 * PURPOSE:
 *   Report declared language features and file discovery through one shared catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/language_cohort.h"

#include "umicom/developer_project/builtin_languages.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include "umicom/platform/process_search_path.h"
#include "umicom/protocol/json_writer.h"
#include "umicom/toolchain/discovery.h"

#include <string.h>

/* Keep priority independent of registry insertion order. Each identifier still
 * resolves to the same pack used by project detection and run/build planning. */
static const char *const language_ids[] = {
    "developer.language.c23", "developer.language.cpp",
    "developer.language.umicom-c", "developer.language.assembly",
    "developer.language.rust", "developer.language.go", "developer.language.zig",
    "developer.language.d", "developer.language.freebasic", "developer.language.pascal",
    "developer.language.fortran", "developer.language.ada", "developer.language.swift",
    "developer.language.java", "developer.language.csharp", "developer.language.kotlin",
    "developer.language.typescript", "developer.language.cpython",
    "developer.language.javascript", "developer.language.php"
};

static const struct {
    const char *name;
    UmiDeveloperProjectLanguageCapability capability;
} capabilities[] = {
    {"syntax", UMI_DEVELOPER_PROJECT_LANGUAGE_SYNTAX},
    {"build", UMI_DEVELOPER_PROJECT_LANGUAGE_BUILD},
    {"run", UMI_DEVELOPER_PROJECT_LANGUAGE_RUN},
    {"debug", UMI_DEVELOPER_PROJECT_LANGUAGE_DEBUG},
    {"test", UMI_DEVELOPER_PROJECT_LANGUAGE_TEST},
    {"format", UMI_DEVELOPER_PROJECT_LANGUAGE_FORMAT},
    {"lsp", UMI_DEVELOPER_PROJECT_LANGUAGE_LSP},
    {"package", UMI_DEVELOPER_PROJECT_LANGUAGE_PACKAGE},
    {"refactor", UMI_DEVELOPER_PROJECT_LANGUAGE_REFACTOR},
    {"symbols", UMI_DEVELOPER_PROJECT_LANGUAGE_SYMBOLS}
};

static const UmiDeveloperProjectLanguagePack *FindLanguage(const char *id)
{
    for (size_t index = 0U; index < umi_developer_project_builtin_language_count(); ++index) {
        const UmiDeveloperProjectLanguagePack *pack =
            umi_developer_project_builtin_language_at(index);
        if (pack != NULL && pack->language_id != NULL && strcmp(pack->language_id, id) == 0)
            return pack;
    }
    return NULL;
}

static const char *HostPlatform(void)
{
#if defined(_WIN32)
    return "windows";
#elif defined(__APPLE__)
    return "macos";
#elif defined(__ANDROID__)
    return "android";
#elif defined(__linux__)
    return "linux";
#elif defined(__FreeBSD__)
    return "freebsd";
#else
    return "other";
#endif
}

static UmiStatus WriteLanguage(UmiJsonWriter *json,
                               const UmiDeveloperProjectLanguagePack *pack,
                               const char *search)
{
    if (pack == NULL || umi_developer_project_language_pack_validate(pack) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_STATE;

    UmiJsonWriterRaw(json, "{\"id\":");
    UmiJsonWriterString(json, pack->language_id);
    UmiJsonWriterRaw(json, ",\"title\":");
    UmiJsonWriterString(json, pack->title);
    UmiJsonWriterRaw(json, ",\"compiled\":");
    UmiJsonWriterBool(json, pack->compiled_language);
    UmiJsonWriterRaw(json, ",\"native\":");
    UmiJsonWriterBool(json, pack->native_language);
    UmiJsonWriterRaw(json, ",\"declared\":{");
    for (size_t index = 0U; index < sizeof capabilities / sizeof capabilities[0]; ++index) {
        if (index != 0U) UmiJsonWriterRaw(json, ",");
        UmiJsonWriterString(json, capabilities[index].name);
        UmiJsonWriterRaw(json, ":");
        UmiJsonWriterBool(json, umi_developer_project_language_pack_has_capability(
            pack, capabilities[index].capability));
    }
    UmiJsonWriterRaw(json, "},\"tools\":[");

    /* Resolve each registered tool against the same search scope. A file hit
     * carries no implication about its identity, version or executable format. */
    for (size_t index = 0U; json->status == UMI_STATUS_OK && index < pack->build_tool_count; ++index) {
        char resolved[UMI_PATH_CAPACITY];
        const char *name = pack->build_tools[index];
        UmiStatus found = UmiToolchainFindInSearchPath(name, search, resolved, sizeof resolved);
        if (found != UMI_STATUS_OK && found != UMI_STATUS_NOT_FOUND) return found;
        if (index != 0U) UmiJsonWriterRaw(json, ",");
        UmiJsonWriterRaw(json, "{\"name\":");
        UmiJsonWriterString(json, name);
        UmiJsonWriterRaw(json, ",\"located\":");
        UmiJsonWriterBool(json, found == UMI_STATUS_OK);
        UmiJsonWriterRaw(json, "}");
    }
    return UmiJsonWriterRaw(json, "]}");
}

UmiStatus UmiProjectLanguageCohortFormatJson(const char *tool_directory,
                                            char *output, size_t capacity)
{
    if (output == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    output[0] = '\0';
    char *inherited = NULL;
    UmiStatus status;
    if (tool_directory == NULL) {
        status = UmiProcessSearchPathRead(&inherited);
    } else {
        status = UmiProcessSearchDirectoryValidate(tool_directory);
        if (status == UMI_STATUS_OK && !umi_fs_is_directory(tool_directory))
            status = UMI_STATUS_NOT_FOUND;
    }
    if (status != UMI_STATUS_OK) return status;
    const char *search = tool_directory == NULL ? inherited : tool_directory;

    /* Build directly into caller-owned storage while withholding incomplete
     * output. Both frontends publish only after the closing delimiter succeeds. */
    UmiJsonWriter json;
    UmiJsonWriterInit(&json, output, capacity);
    UmiJsonWriterRaw(&json, "{\"schema\":1,\"platform\":");
    UmiJsonWriterString(&json, HostPlatform());
    UmiJsonWriterRaw(&json, ",\"evidence\":\"tool-file-lookup\",\"tool_selection\":");
    UmiJsonWriterString(&json, tool_directory == NULL ? "inherited-path" : "explicit-directory");
    UmiJsonWriterRaw(&json, ",\"languages\":[");
    status = json.status;
    for (size_t index = 0U; status == UMI_STATUS_OK && index < sizeof language_ids / sizeof language_ids[0]; ++index) {
        if (index != 0U) UmiJsonWriterRaw(&json, ",");
        status = WriteLanguage(&json, FindLanguage(language_ids[index]), search);
    }
    if (status == UMI_STATUS_OK) status = UmiJsonWriterRaw(&json, "]}");
    UmiProcessSearchPathFree(inherited);
    if (status != UMI_STATUS_OK) output[0] = '\0';
    return status;
}
