/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/target_catalogue_tree.inc
 * PURPOSE: Read large configured target snapshots with owned trees and linear sibling traversal.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/json_tree.h"
struct UmiProjectTargetCatalogue
{
    UmiProjectTargetSummary summary;
    UmiProjectTargetChoice *choices;
};
typedef struct TargetDocument
{
    UmiJsonTree *json;
} TargetDocument;
typedef struct TargetIndex
{
    char name[UMI_BUILD_PATH_CAPACITY];
    char error[UMI_BUILD_PATH_CAPACITY];
} TargetIndex;
/* Decode names before matching so an escaped spelling cannot hide a duplicate
 * field. An unrelated longer key cannot equal any of our short property names.
 * -1 means absent; -2 means malformed or duplicated, never an optional default. */
static int TargetField(const UmiJsonTree *doc, int object, const char *name)
{
    int node;
    UmiStatus status = UmiJsonTreeMember(doc, object, name, &node);
    return status == UMI_STATUS_OK ? node : status == UMI_STATUS_NOT_FOUND ? -1 : -2;
}
static UmiStatus TargetText(const UmiJsonTree *doc, int object, const char *field, char *out, size_t capacity,
                            bool required, bool multiline)
{
    int token = TargetField(doc, object, field);
    if (token == -1)
        return required ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK;
    if (token < 0 || UmiJsonTreeKind(doc, token) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        return UMI_STATUS_PARSE_ERROR;
    UmiStatus status = UmiJsonTreeText(doc, token, out, capacity);
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
static UmiStatus TargetNewest(const char *reply, TargetIndex *out, const UmiCancellationToken *cancel)
{
    memset(out, 0, sizeof(*out));
    UmiDirectoryWalkOptions options = umi_directory_walk_options_default();
    options.recursive = 0;
    options.include_hidden = 1;
    options.include_directories = 1;
    /* The walk is nonrecursive. Include link names so a newer linked index is
     * rejected by the regular-file reader, rather than hiding an older reply. */
    options.follow_symbolic_links = 1;
    UmiStatus status = UmiDirectoryWalkCancellable(reply, &options, TargetVisit, out, cancel);
    if (status != UMI_STATUS_OK)
        return status;
    /* CMake uses the same sortable suffix for success and error indexes. An
     * error at least as recent as the success must not look like a fresh build. */
    if (out->error[0] != '\0' && (out->name[0] == '\0' || strcmp(out->error + 6U, out->name + 6U) >= 0))
        return UMI_STATUS_INVALID_STATE;
    return out->name[0] != '\0' ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}
/* Input bytes are copied once into the owned tree. Releasing the temporary
 * read buffer immediately avoids keeping two copies throughout discovery. */
static UmiStatus TargetReadDocument(const char *reply, const char *leaf, const UmiCancellationToken *cancel,
                                    TargetDocument **out)
{
    *out = NULL;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    if (!TargetLeaf(leaf))
        return UMI_STATUS_PARSE_ERROR;
    char path[UMI_BUILD_PATH_CAPACITY];
    UmiStatus status = umi_path_join(reply, leaf, path, sizeof(path));
    TargetDocument *doc = calloc(1U, sizeof(*doc));
    if (doc == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiInputFileRead(path, UMI_PROJECT_TARGET_DOCUMENT_LIMIT, &bytes, &size);
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeCreate(bytes, size, NULL, cancel, &doc->json);
    UmiInputFileFree(bytes);
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(doc->json, 0) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        *out = doc;
    else
    {
        UmiJsonTreeDestroy(doc->json);
        free(doc);
    }
    return status;
}
static void TargetDocumentFree(TargetDocument *doc)
{
    if (doc != NULL)
    {
        UmiJsonTreeDestroy(doc->json);
        free(doc);
    }
}
static UmiStatus TargetArray(const UmiJsonTree *doc, int object, const char *name, int *out)
{
    int array = TargetField(doc, object, name);
    if (array < 0 || UmiJsonTreeKind(doc, array) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        return UMI_STATUS_PARSE_ERROR;
    *out = array;
    return UMI_STATUS_OK;
}
static UmiStatus TargetModelVersion(const UmiJsonTree *doc, int object)
{
    char kind[32] = {0};
    UmiStatus status = TargetText(doc, object, "kind", kind, sizeof(kind), true, false);
    int version = TargetField(doc, object, "version");
    int major = TargetField(doc, version, "major");
    int minor = TargetField(doc, version, "minor");
    int64_t majorValue = 0, minorValue = 0;
    if (status != UMI_STATUS_OK || strcmp(kind, "codemodel") != 0 || major < 0 || minor < 0 ||
        UmiJsonTreeInteger(doc, major, &majorValue) != UMI_STATUS_OK || majorValue != 2 ||
        UmiJsonTreeInteger(doc, minor, &minorValue) != UMI_STATUS_OK || minorValue < 0)
        return UMI_STATUS_PARSE_ERROR;
    return UMI_STATUS_OK;
}
static UmiStatus TargetModelReference(const UmiJsonTree *doc, char *out)
{
    int array;
    UmiStatus status = TargetArray(doc, 0, "objects", &array);
    if (status != UMI_STATUS_OK)
        return status;
    bool found = false;
    for (int object = UmiJsonTreeFirst(doc, array); object >= 0; object = UmiJsonTreeNext(doc, object))
    {
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
static UmiStatus TargetConfiguration(const UmiJsonTree *doc, const char *requested,
                                     UmiProjectTargetCatalogue *catalogue, int *out)
{
    int array;
    UmiStatus status = TargetArray(doc, 0, "configurations", &array);
    if (status != UMI_STATUS_OK)
        return status;
    size_t count = UmiJsonTreeCount(doc, array);
    if (count > 32U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    int selected = -1;
    char names[32][UMI_BUILD_NAME_CAPACITY] = {{0}};
    int item = UmiJsonTreeFirst(doc, array);
    for (size_t i = 0U; i < count; ++i, item = UmiJsonTreeNext(doc, item))
    {
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
static UmiStatus TargetRow(const UmiJsonTree *model, int reference, const char *reply,
                           UmiProjectTargetCatalogue *catalogue, const UmiCancellationToken *cancel)
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
    TargetDocument *target = NULL;
    status = TargetReadDocument(reply, leaf, cancel, &target);
    if (status != UMI_STATUS_OK)
        return status;
    UmiJsonTree *doc = target->json;
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
        if (field < 0 || UmiJsonTreeBoolean(doc, field, &value) != UMI_STATUS_OK)
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
            (artifacts >= 0 && UmiJsonTreeKind(doc, artifacts) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY))
            status = UMI_STATUS_PARSE_ERROR;
        size_t count = artifacts >= 0 ? UmiJsonTreeCount(doc, artifacts) : 0U;
        if (count > 32U)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        int artifact = UmiJsonTreeFirst(doc, artifacts);
        for (size_t i = 0U; status == UMI_STATUS_OK && i < count;
             ++i, artifact = UmiJsonTreeNext(doc, artifact))
        {
            char path[UMI_BUILD_PATH_CAPACITY] = {0}, absolute[UMI_BUILD_PATH_CAPACITY],
                 basename[UMI_BUILD_PATH_CAPACITY];
            status = TargetText(doc, artifact, "path", path, sizeof(path), true, false);
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
            (launchers >= 0 && UmiJsonTreeKind(doc, launchers) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY))
            status = UMI_STATUS_PARSE_ERROR;
        else if (launchers >= 0 && UmiJsonTreeCount(doc, launchers) != 0U)
            choice.program[0] = '\0';
    }
    if (status == UMI_STATUS_OK)
        catalogue->choices[catalogue->summary.count++] = choice;
    TargetDocumentFree(target);
    return status;
}
/* Sorting pointer views keeps declaration order visible and makes duplicate
 * name/identity checks O(n log n), without copying large path-bearing rows. */
static int TargetCompareName(const void *left, const void *right)
{
    const UmiProjectTargetChoice *const *a = left, *const *b = right;
    return strcmp((*a)->name, (*b)->name);
}
static int TargetCompareIdentity(const void *left, const void *right)
{
    const UmiProjectTargetChoice *const *a = left, *const *b = right;
    return strcmp((*a)->identity, (*b)->identity);
}
static UmiStatus TargetUnique(const UmiProjectTargetCatalogue *catalogue)
{
    size_t count = catalogue->summary.count;
    if (count < 2U)
        return UMI_STATUS_OK;
    const UmiProjectTargetChoice **order = malloc(count * sizeof(*order));
    if (order == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t i = 0U; i < count; ++i)
        order[i] = &catalogue->choices[i];
    UmiStatus status = UMI_STATUS_OK;
    qsort(order, count, sizeof(*order), TargetCompareName);
    for (size_t i = 1U; i < count; ++i)
        if (strcmp(order[i - 1U]->name, order[i]->name) == 0)
            status = UMI_STATUS_ALREADY_EXISTS;
    qsort(order, count, sizeof(*order), TargetCompareIdentity);
    for (size_t i = 1U; i < count; ++i)
        if (strcmp(order[i - 1U]->identity, order[i]->identity) == 0)
            status = UMI_STATUS_ALREADY_EXISTS;
    free(order);
    return status;
}
UmiStatus UmiProjectTargetCatalogueRead(const char *sourceDirectory, const char *buildDirectory,
                                        const char *configuration, UmiProjectTargetCatalogue **out)
{
    return UmiProjectTargetCatalogueReadCancellable(sourceDirectory, buildDirectory, configuration, NULL,
                                                    out);
}
/* Cancellation belongs to the shared reader, so CLI and native consumers use
 * the same no-partial-result policy when abandoning a large project scan. */
UmiStatus UmiProjectTargetCatalogueReadCancellable(const char *sourceDirectory, const char *buildDirectory,
                                                   const char *configuration,
                                                   const UmiCancellationToken *cancel,
                                                   UmiProjectTargetCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
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
        status = TargetNewest(reply, &index, cancel);
    if (status == UMI_STATUS_OK)
        status = TargetReadDocument(reply, index.name, cancel, &document);
    if (status == UMI_STATUS_OK)
        status = TargetModelReference(document->json, modelLeaf);
    if (status == UMI_STATUS_OK)
        status = TargetReadDocument(reply, modelLeaf, cancel, &model);
    if (status == UMI_STATUS_OK)
        status = TargetModelVersion(model->json, 0);
    if (status == UMI_STATUS_OK)
    {
        char source[UMI_BUILD_PATH_CAPACITY] = {0}, build[UMI_BUILD_PATH_CAPACITY] = {0};
        int paths = TargetField(model->json, 0, "paths");
        status = TargetText(model->json, paths, "source", source, sizeof(source), true, false);
        if (status == UMI_STATUS_OK)
            status = TargetText(model->json, paths, "build", build, sizeof(build), true, false);
        if (status == UMI_STATUS_OK && (!umi_path_is_absolute(source) || !umi_path_is_absolute(build) ||
                                        !umi_path_equal(source, summary->source_directory) ||
                                        !umi_path_equal(build, summary->build_directory)))
            status = UMI_STATUS_INVALID_STATE;
    }
    int targets = -1;
    if (status == UMI_STATUS_OK)
        status = TargetConfiguration(model->json, configuration, catalogue, &targets);
    size_t count = status == UMI_STATUS_OK ? UmiJsonTreeCount(model->json, targets) : 0U;
    if (count > UMI_PROJECT_TARGET_LIMIT)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK && count != 0U)
    {
        catalogue->choices = calloc(count, sizeof(*catalogue->choices));
        if (catalogue->choices == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
    }
    /* Check the complete reference list before opening target files. Duplicate
     * names or identities are ambiguous even when the repeated file is absent
     * or names another target. Reuse the allocated rows for this first pass. */
    int reference = status == UMI_STATUS_OK ? UmiJsonTreeFirst(model->json, targets) : -1;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count;
         ++i, reference = UmiJsonTreeNext(model->json, reference))
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        UmiProjectTargetChoice *choice = &catalogue->choices[i];
        status = TargetText(model->json, reference, "name", choice->name, sizeof(choice->name), true, false);
        if (status == UMI_STATUS_OK)
            status = TargetText(model->json, reference, "id", choice->identity, sizeof(choice->identity),
                                true, false);
        if (status == UMI_STATUS_OK)
            ++summary->count;
    }
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = TargetUnique(catalogue);
    summary->count = 0U;
    reference = status == UMI_STATUS_OK ? UmiJsonTreeFirst(model->json, targets) : -1;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count;
         ++i, reference = UmiJsonTreeNext(model->json, reference))
        status = TargetRow(model->json, reference, reply, catalogue, cancel);
    /* Recheck the index before publishing. If generation replaced the snapshot,
     * the caller refreshes instead of mixing target files from different runs. */
    if (status == UMI_STATUS_OK)
        status = TargetNewest(reply, &after, cancel);
    if (status == UMI_STATUS_OK && strcmp(index.name, after.name) != 0)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = umi_path_join(reply, index.name, summary->index_file, sizeof(summary->index_file));
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    TargetDocumentFree(document);
    TargetDocumentFree(model);
    if (status == UMI_STATUS_OK)
        *out = catalogue;
    else
        UmiProjectTargetCatalogueDestroy(catalogue);
    return status;
}
void UmiProjectTargetCatalogueDestroy(UmiProjectTargetCatalogue *catalogue)
{
    if (catalogue != NULL)
    {
        free(catalogue->choices);
        free(catalogue);
    }
}
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
