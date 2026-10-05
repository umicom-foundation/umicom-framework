/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/target_catalogue.c
 * PURPOSE: Keep configured target discovery and safe selection in the shared developer service.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_project/target_catalogue.h"
#include "umicom/language_runtime/json_document.h"
#include "umicom/language_runtime/json_text.h"
#include "umicom/platform/directory.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/output_file.h"
#include <stdlib.h>
#include <string.h>

/* Heap-backed JSON trees and allocated target rows replace the fixed document
 * and catalogue arrays below. This permits larger configured projects without
 * enlarging every small-message parser. The former implementation is retained
 * for engineering review; public entry points are supplied by the included
 * implementation after this disabled block. */
#if 0
struct UmiProjectTargetCatalogue
{
    UmiProjectTargetSummary summary;
    UmiProjectTargetChoice choices[UMI_PROJECT_TARGET_LIMIT];
};
typedef struct TargetDocument
{
    unsigned char *bytes;
    UmiLanguageRuntimeJsonDocument json;
} TargetDocument;
typedef struct TargetIndex
{
    char name[UMI_BUILD_PATH_CAPACITY];
    char error[UMI_BUILD_PATH_CAPACITY];
} TargetIndex;
/* Decode names before matching so an escaped spelling cannot hide a duplicate
 * field. An unrelated longer key cannot equal any of our short property names.
 * -1 means absent; -2 means malformed or duplicated, never an optional default. */
static int TargetField(const UmiLanguageRuntimeJsonDocument *doc, int object, const char *name)
{
    if (object < 0 || doc->tokens[object].type != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        return -2;
    int found = -1;
    size_t count = umi_language_runtime_json_object_count(doc, object);
    for (size_t i = 0U; i < count; ++i)
    {
        int key, value;
        char decoded[64];
        if (umi_language_runtime_json_object_entry_at(doc, object, i, &key, &value) != UMI_STATUS_OK)
            return -2;
        UmiStatus status = UmiLanguageRuntimeJsonText(doc, key, decoded, sizeof(decoded));
        if (status == UMI_STATUS_CAPACITY_EXCEEDED)
            continue;
        if (status != UMI_STATUS_OK)
            return -2;
        if (strcmp(decoded, name) == 0)
        {
            if (found >= 0)
                return -2;
            found = value;
        }
    }
    return found;
}
static UmiStatus TargetText(const UmiLanguageRuntimeJsonDocument *doc, int object, const char *field,
                            char *out, size_t capacity, bool required, bool multiline)
{
    int token = TargetField(doc, object, field);
    if (token == -1)
        return required ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK;
    if (token < 0 || doc->tokens[token].type != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        return UMI_STATUS_PARSE_ERROR;
    UmiStatus status = UmiLanguageRuntimeJsonText(doc, token, out, capacity);
    if (status != UMI_STATUS_OK)
        return status;
    if (required && out[0] == '\0')
        return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; out[i] != '\0'; ++i)
    {
        unsigned char byte = (unsigned char)out[i];
        if ((byte < 0x20U && !(multiline && (byte == '\n' || byte == '\r' || byte == '\t'))) || byte == 0x7fU)
            return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}

/* A reference is a file name inside reply/, never a path supplied by metadata.
 * This prevents ../ traversal and Windows streams before any file is opened. */
static bool TargetLeaf(const char *name)
{
    size_t length = strlen(name);
    if (length <= 5U || strcmp(name + length - 5U, ".json") != 0)
        return false;
    for (size_t i = 0U; i < length; ++i)
    {
        unsigned char c = (unsigned char)name[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' ||
              c == '_' || c == '.'))
            return false;
    }
    return name[0] != '.' && strstr(name, "..") == NULL;
}
static UmiStatus TargetDirectory(const char *path, char *out)
{
    if (path == NULL || !umi_path_is_absolute(path))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_path_normalise(path, out, UMI_BUILD_PATH_CAPACITY);
    if (status == UMI_STATUS_OK)
        status = UmiOutputFileValidatePath(out);
    return status;
}
static UmiStatus TargetVisit(const UmiFileInfo *info, void *context)
{
    TargetIndex *index = context;
    /* Keep the newest name even if its leaf is a link or directory. The regular
     * file reader will reject it; silently falling back would expose old data. */
    if (!TargetLeaf(info->name))
        return UMI_STATUS_OK;
    char *destination = strncmp(info->name, "index-", 6U) == 0   ? index->name
                        : strncmp(info->name, "error-", 6U) == 0 ? index->error
                                                                 : NULL;
    if (destination != NULL && strcmp(destination, info->name) < 0)
        memcpy(destination, info->name, strlen(info->name) + 1U);
    return UMI_STATUS_OK;
}
static UmiStatus TargetNewest(const char *reply, TargetIndex *out)
{
    memset(out, 0, sizeof(*out));
    UmiDirectoryWalkOptions options = umi_directory_walk_options_default();
    options.recursive = 0;
    options.include_hidden = 1;
    options.include_directories = 1;
    /* The walk is nonrecursive. Include link names so a newer linked index is
     * rejected by the regular-file reader, rather than hiding an older reply. */
    options.follow_symbolic_links = 1;
    UmiStatus status = umi_directory_walk(reply, &options, TargetVisit, out);
    if (status != UMI_STATUS_OK)
        return status;
    /* CMake uses the same sortable suffix for success and error indexes. An
     * error at least as recent as the success must not look like a fresh build. */
    if (out->error[0] != '\0' && (out->name[0] == '\0' || strcmp(out->error + 6U, out->name + 6U) >= 0))
        return UMI_STATUS_INVALID_STATE;
    return out->name[0] != '\0' ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}
static UmiStatus TargetReadDocument(const char *reply, const char *leaf, TargetDocument **out)
{
    *out = NULL;
    if (!TargetLeaf(leaf))
        return UMI_STATUS_PARSE_ERROR;
    char path[UMI_BUILD_PATH_CAPACITY];
    UmiStatus status = umi_path_join(reply, leaf, path, sizeof(path));
    TargetDocument *doc = calloc(1U, sizeof(*doc));
    if (doc == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    size_t size = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiInputFileRead(path, UMI_PROJECT_TARGET_DOCUMENT_LIMIT, &doc->bytes, &size);
    if (status == UMI_STATUS_OK && (size == 0U || memchr(doc->bytes, '\0', size) != NULL))
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = UmiLanguageRuntimeJsonParseComplete((const char *)doc->bytes, &doc->json);
    if (status == UMI_STATUS_OK && doc->json.tokens[0].type != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        *out = doc;
    else
    {
        UmiInputFileFree(doc->bytes);
        free(doc);
    }
    return status;
}
static void TargetDocumentFree(TargetDocument *doc)
{
    if (doc != NULL)
    {
        UmiInputFileFree(doc->bytes);
        free(doc);
    }
}
static UmiStatus TargetArray(const UmiLanguageRuntimeJsonDocument *doc, int object, const char *name,
                             int *out)
{
    int array = TargetField(doc, object, name);
    if (array < 0 || doc->tokens[array].type != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        return UMI_STATUS_PARSE_ERROR;
    *out = array;
    return UMI_STATUS_OK;
}
static UmiStatus TargetModelVersion(const UmiLanguageRuntimeJsonDocument *doc, int object)
{
    char kind[32] = {0};
    UmiStatus status = TargetText(doc, object, "kind", kind, sizeof(kind), true, false);
    int version = TargetField(doc, object, "version");
    int major = TargetField(doc, version, "major");
    int minor = TargetField(doc, version, "minor");
    int64_t majorValue = 0, minorValue = 0;
    if (status != UMI_STATUS_OK || strcmp(kind, "codemodel") != 0 || major < 0 || minor < 0 ||
        umi_language_runtime_json_int64(doc, major, &majorValue) != UMI_STATUS_OK || majorValue != 2 ||
        umi_language_runtime_json_int64(doc, minor, &minorValue) != UMI_STATUS_OK || minorValue < 0)
        return UMI_STATUS_PARSE_ERROR;
    return UMI_STATUS_OK;
}
static UmiStatus TargetModelReference(const UmiLanguageRuntimeJsonDocument *doc, char *out)
{
    int array;
    UmiStatus status = TargetArray(doc, 0, "objects", &array);
    if (status != UMI_STATUS_OK)
        return status;
    bool found = false;
    for (size_t i = 0U; i < umi_language_runtime_json_array_count(doc, array); ++i)
    {
        int object = umi_language_runtime_json_array_at(doc, array, i);
        char kind[64] = {0};
        status = TargetText(doc, object, "kind", kind, sizeof(kind), true, false);
        if (status != UMI_STATUS_OK)
            return status;
        if (strcmp(kind, "codemodel") != 0)
            continue;
        if (found)
            return UMI_STATUS_ALREADY_EXISTS;
        status = TargetModelVersion(doc, object);
        if (status == UMI_STATUS_OK)
            status = TargetText(doc, object, "jsonFile", out, UMI_BUILD_PATH_CAPACITY, true, false);
        if (status != UMI_STATUS_OK)
            return status;
        found = true;
    }
    return found ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}
/* Only the requested configuration is loaded. Do not silently pick the first
 * Debug/Release entry of a multi-configuration build. An unnamed single-config
 * build has no configuration to choose, so the profile's name is left intact. */
static UmiStatus TargetConfiguration(const UmiLanguageRuntimeJsonDocument *doc, const char *requested,
                                     UmiProjectTargetCatalogue *catalogue, int *out)
{
    int array;
    UmiStatus status = TargetArray(doc, 0, "configurations", &array);
    if (status != UMI_STATUS_OK)
        return status;
    size_t count = umi_language_runtime_json_array_count(doc, array);
    if (count > 32U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    int selected = -1;
    char names[32][UMI_BUILD_NAME_CAPACITY] = {{0}};
    for (size_t i = 0U; i < count; ++i)
    {
        int item = umi_language_runtime_json_array_at(doc, array, i);
        status = TargetText(doc, item, "name", names[i], sizeof(names[i]), false, false);
        if (TargetField(doc, item, "name") < 0)
            return UMI_STATUS_PARSE_ERROR;
        if (status != UMI_STATUS_OK)
            return status;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(names[i], names[j]) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
        if (strcmp(names[i], requested) == 0 || (count == 1U && names[i][0] == '\0'))
        {
            selected = item;
            memcpy(catalogue->summary.configuration, names[i], strlen(names[i]) + 1U);
        }
    }
    if (selected < 0)
        return UMI_STATUS_NOT_FOUND;
    return TargetArray(doc, selected, "targets", out);
}
static UmiStatus TargetRow(const UmiLanguageRuntimeJsonDocument *model, int reference, const char *reply,
                           UmiProjectTargetCatalogue *catalogue)
{
    UmiProjectTargetChoice choice = {0};
    char leaf[UMI_BUILD_PATH_CAPACITY] = {0}, name[UMI_BUILD_NAME_CAPACITY] = {0}, identity[512] = {0};
    UmiStatus status = TargetText(model, reference, "name", choice.name, sizeof(choice.name), true, false);
    if (status == UMI_STATUS_OK)
        status = TargetText(model, reference, "id", choice.identity, sizeof(choice.identity), true, false);
    if (status == UMI_STATUS_OK)
        status = TargetText(model, reference, "jsonFile", leaf, sizeof(leaf), true, false);
    if (status != UMI_STATUS_OK)
        return status;
    for (size_t i = 0U; i < catalogue->summary.count; ++i)
        if (strcmp(choice.name, catalogue->choices[i].name) == 0 ||
            strcmp(choice.identity, catalogue->choices[i].identity) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    TargetDocument *target = NULL;
    status = TargetReadDocument(reply, leaf, &target);
    if (status != UMI_STATUS_OK)
        return status;
    UmiLanguageRuntimeJsonDocument *doc = &target->json;
    status = TargetText(doc, 0, "name", name, sizeof(name), true, false);
    if (status == UMI_STATUS_OK)
        status = TargetText(doc, 0, "id", identity, sizeof(identity), true, false);
    if (status == UMI_STATUS_OK)
        status = TargetText(doc, 0, "type", choice.type, sizeof(choice.type), true, false);
    if (status == UMI_STATUS_OK && (strcmp(name, choice.name) != 0 || strcmp(identity, choice.identity) != 0))
        status = UMI_STATUS_INVALID_STATE;
    const char *flags[] = {"abstract", "imported"};
    for (size_t i = 0U; status == UMI_STATUS_OK && i < 2U; ++i)
    {
        int field = TargetField(doc, 0, flags[i]), value = 0;
        if (field == -1)
            continue;
        if (field < 0 || umi_language_runtime_json_bool(doc, field, &value) != UMI_STATUS_OK)
            status = UMI_STATUS_PARSE_ERROR;
        else if (value)
            status = UMI_STATUS_INVALID_STATE;
    }
    const char *types[] = {"EXECUTABLE",     "STATIC_LIBRARY",    "SHARED_LIBRARY", "MODULE_LIBRARY",
                           "OBJECT_LIBRARY", "INTERFACE_LIBRARY", "UTILITY"};
    bool known = false;
    for (size_t i = 0U; i < sizeof(types) / sizeof(types[0]); ++i)
        if (strcmp(choice.type, types[i]) == 0)
            known = true;
    if (status == UMI_STATUS_OK && !known)
        status = UMI_STATUS_PARSE_ERROR;
    choice.executable = strcmp(choice.type, "EXECUTABLE") == 0;
    if (status == UMI_STATUS_OK && choice.executable)
    {
        char diskName[UMI_BUILD_PATH_CAPACITY] = {0};
        status = TargetText(doc, 0, "nameOnDisk", diskName, sizeof(diskName), false, false);
        int artifacts = TargetField(doc, 0, "artifacts");
        size_t matches = 0U;
        if (artifacts < -1 ||
            (artifacts >= 0 && doc->tokens[artifacts].type != UMI_LANGUAGE_RUNTIME_JSON_ARRAY))
            status = UMI_STATUS_PARSE_ERROR;
        size_t count = artifacts >= 0 ? umi_language_runtime_json_array_count(doc, artifacts) : 0U;
        if (count > 32U)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i)
        {
            char path[UMI_BUILD_PATH_CAPACITY] = {0}, absolute[UMI_BUILD_PATH_CAPACITY],
                 basename[UMI_BUILD_PATH_CAPACITY];
            status = TargetText(doc, umi_language_runtime_json_array_at(doc, artifacts, i), "path", path,
                                sizeof(path), true, false);
#ifdef _WIN32
            /* Do not turn a drive-relative or root-relative artifact into an
             * apparently absolute path using an unrelated build directory. */
            if (status == UMI_STATUS_OK && !umi_path_is_absolute(path) &&
                (path[0] == '/' || path[0] == '\\' || strchr(path, ':') != NULL))
                status = UMI_STATUS_PARSE_ERROR;
#endif
            if (status == UMI_STATUS_OK)
                status =
                    umi_path_absolute(path, catalogue->summary.build_directory, absolute, sizeof(absolute));
            if (status == UMI_STATUS_OK)
                status = UmiOutputFileValidatePath(absolute);
            if (status == UMI_STATUS_OK)
                status = umi_path_basename(absolute, basename, sizeof(basename));
            if (status == UMI_STATUS_OK && diskName[0] != '\0' && strcmp(basename, diskName) == 0)
            {
                ++matches;
                memcpy(choice.program, absolute, strlen(absolute) + 1U);
            }
        }
        if (matches != 1U)
            choice.program[0] = '\0';
        /* Cross-target emulators and test launchers require their own launch
         * contract. Keep the build target, but do not imply direct execution. */
        int launchers = TargetField(doc, 0, "launchers");
        if (launchers < -1 ||
            (launchers >= 0 && doc->tokens[launchers].type != UMI_LANGUAGE_RUNTIME_JSON_ARRAY))
            status = UMI_STATUS_PARSE_ERROR;
        else if (launchers >= 0 && umi_language_runtime_json_array_count(doc, launchers) != 0U)
            choice.program[0] = '\0';
    }
    if (status == UMI_STATUS_OK)
        catalogue->choices[catalogue->summary.count++] = choice;
    TargetDocumentFree(target);
    return status;
}
UmiStatus UmiProjectTargetCatalogueRead(const char *sourceDirectory, const char *buildDirectory,
                                        const char *configuration, UmiProjectTargetCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (configuration == NULL || strlen(configuration) >= UMI_BUILD_NAME_CAPACITY)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiProjectTargetCatalogue *catalogue = calloc(1U, sizeof(*catalogue));
    if (catalogue == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiProjectTargetSummary *summary = &catalogue->summary;
    UmiStatus status = TargetDirectory(sourceDirectory, summary->source_directory);
    if (status == UMI_STATUS_OK)
        status = TargetDirectory(buildDirectory, summary->build_directory);
    char reply[UMI_BUILD_PATH_CAPACITY], modelLeaf[UMI_BUILD_PATH_CAPACITY] = {0};
    TargetIndex index, after;
    TargetDocument *document = NULL, *model = NULL;
    if (status == UMI_STATUS_OK)
        status = umi_path_join(summary->build_directory, ".cmake/api/v1/reply", reply, sizeof(reply));
    if (status == UMI_STATUS_OK)
        status = TargetNewest(reply, &index);
    if (status == UMI_STATUS_OK)
        status = TargetReadDocument(reply, index.name, &document);
    if (status == UMI_STATUS_OK)
        status = TargetModelReference(&document->json, modelLeaf);
    if (status == UMI_STATUS_OK)
        status = TargetReadDocument(reply, modelLeaf, &model);
    if (status == UMI_STATUS_OK)
        status = TargetModelVersion(&model->json, 0);
    if (status == UMI_STATUS_OK)
    {
        char source[UMI_BUILD_PATH_CAPACITY] = {0}, build[UMI_BUILD_PATH_CAPACITY] = {0};
        int paths = TargetField(&model->json, 0, "paths");
        status = TargetText(&model->json, paths, "source", source, sizeof(source), true, false);
        if (status == UMI_STATUS_OK)
            status = TargetText(&model->json, paths, "build", build, sizeof(build), true, false);
        if (status == UMI_STATUS_OK && (!umi_path_is_absolute(source) || !umi_path_is_absolute(build) ||
                                        !umi_path_equal(source, summary->source_directory) ||
                                        !umi_path_equal(build, summary->build_directory)))
            status = UMI_STATUS_INVALID_STATE;
    }
    int targets = -1;
    if (status == UMI_STATUS_OK)
        status = TargetConfiguration(&model->json, configuration, catalogue, &targets);
    size_t count =
        status == UMI_STATUS_OK ? umi_language_runtime_json_array_count(&model->json, targets) : 0U;
    if (count > UMI_PROJECT_TARGET_LIMIT)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i)
        status = TargetRow(&model->json, umi_language_runtime_json_array_at(&model->json, targets, i), reply,
                           catalogue);
    /* Recheck the index before publishing. If generation replaced the snapshot,
     * the caller refreshes instead of mixing target files from different runs. */
    if (status == UMI_STATUS_OK)
        status = TargetNewest(reply, &after);
    if (status == UMI_STATUS_OK && strcmp(index.name, after.name) != 0)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = umi_path_join(reply, index.name, summary->index_file, sizeof(summary->index_file));
    TargetDocumentFree(document);
    TargetDocumentFree(model);
    if (status == UMI_STATUS_OK)
        *out = catalogue;
    else
        free(catalogue);
    return status;
}
void UmiProjectTargetCatalogueDestroy(UmiProjectTargetCatalogue *catalogue) { free(catalogue); }
UmiStatus UmiProjectTargetCatalogueSummary(const UmiProjectTargetCatalogue *catalogue,
                                           UmiProjectTargetSummary *out)
{
    if (catalogue == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = catalogue->summary;
    return UMI_STATUS_OK;
}
UmiStatus UmiProjectTargetCatalogueAt(const UmiProjectTargetCatalogue *catalogue, size_t index,
                                      UmiProjectTargetChoice *out)
{
    if (catalogue == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= catalogue->summary.count)
        return UMI_STATUS_NOT_FOUND;
    *out = catalogue->choices[index];
    return UMI_STATUS_OK;
}
UmiStatus UmiProjectTargetCatalogueSelect(const UmiProjectTargetCatalogue *catalogue, size_t index,
                                          UmiProjectTargetSelection selection, const UmiBuildProfile *profile,
                                          UmiBuildProfile *out)
{
    if (catalogue == NULL || out == NULL ||
        (selection != UMI_PROJECT_TARGET_BUILD && selection != UMI_PROJECT_TARGET_PROGRAM))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_build_profile_validate(profile, NULL, 0U);
    if (status != UMI_STATUS_OK)
        return status;
    if (index >= catalogue->summary.count)
        return UMI_STATUS_NOT_FOUND;
    char build[UMI_BUILD_PATH_CAPACITY];
    if (!umi_path_is_absolute(profile->source_directory))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_path_absolute(profile->build_directory, profile->source_directory, build, sizeof(build));
    if (status != UMI_STATUS_OK)
        return status;
    if (!umi_path_equal(profile->source_directory, catalogue->summary.source_directory) ||
        !umi_path_equal(build, catalogue->summary.build_directory) ||
        (catalogue->summary.configuration[0] != '\0' &&
         strcmp(profile->configuration, catalogue->summary.configuration) != 0))
        return UMI_STATUS_INVALID_STATE;
    const UmiProjectTargetChoice *choice = &catalogue->choices[index];
    UmiBuildProfile candidate = *profile;
    if (selection == UMI_PROJECT_TARGET_BUILD)
    {
        if (profile->build_preset[0] != '\0')
            return UMI_STATUS_INVALID_STATE;
        memcpy(candidate.build_target, choice->name, strlen(choice->name) + 1U);
    }
    else
    {
        if (!choice->executable || choice->program[0] == '\0')
            return UMI_STATUS_INVALID_STATE;
        memcpy(candidate.run_program, choice->program, strlen(choice->program) + 1U);
    }
    *out = candidate;
    return UMI_STATUS_OK;
}

#endif
#include "target_catalogue_tree.inc"
