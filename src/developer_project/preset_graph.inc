/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/preset_graph.inc
 * PURPOSE: Discover included CMake preset declarations and their inherited review metadata without executing tools.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/json_tree.h"
#define PRESET_DOCUMENT_CAPACITY 64U
#define PRESET_DOCUMENT_BYTES (2U * 1024U * 1024U)
#define PRESET_TOTAL_BYTES (8U * 1024U * 1024U)

typedef struct PresetDocumentNode
{
    char path[UMI_BUILD_PATH_CAPACITY];
    UmiJsonTree *tree;
    uint64_t visible;
    int state;
    bool user, standard;
} PresetDocumentNode;
typedef struct PresetChoiceNode
{
    size_t file;
    int parents, state;
    bool configure, binary;
    /* Null blocks this preset's own inheritance but is not passed to children. */
    int condition;
} PresetChoiceNode;
typedef struct PresetGraph
{
    UmiProjectPresetCatalogue *catalogue;
    PresetDocumentNode files[PRESET_DOCUMENT_CAPACITY];
    PresetChoiceNode choices[UMI_PROJECT_PRESET_LIMIT];
    size_t file_count, bytes;
    char root[UMI_BUILD_PATH_CAPACITY], problem[UMI_BUILD_PATH_CAPACITY];
    const UmiCancellationToken *cancel;
} PresetGraph;

static UmiStatus PresetGraphCancelled(const PresetGraph *graph)
{
    return umi_cancellation_token_is_requested(graph->cancel) ? UMI_STATUS_CANCELLED : UMI_STATUS_OK;
}
/* Missing metadata is optional; a repeated decoded member is malformed. Never
 * pick whichever duplicated field happens to occur first in the JSON file. */
static UmiStatus PresetGraphMember(const UmiJsonTree *tree, int object, const char *name, int *out)
{
    *out = -1;
    UmiStatus status = UmiJsonTreeMember(tree, object, name, out);
    if (status == UMI_STATUS_NOT_FOUND)
        return UMI_STATUS_OK;
    return status == UMI_STATUS_ALREADY_EXISTS ? UMI_STATUS_PARSE_ERROR : status;
}
static UmiStatus PresetGraphText(const UmiJsonTree *tree, int token, char *out, size_t capacity,
                                 bool required, bool multiline)
{
    if (token == -1)
        return required ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_OK;
    UmiStatus status = UmiJsonTreeText(tree, token, out, capacity);
    if (status != UMI_STATUS_OK)
        return status == UMI_STATUS_INVALID_ARGUMENT ? UMI_STATUS_PARSE_ERROR : status;
    if (required && out[0] == '\0')
        return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; out[i] != '\0'; ++i)
    {
        unsigned char value = (unsigned char)out[i];
        if ((value < 32U && !(multiline && (value == '\n' || value == '\r' || value == '\t'))) ||
            value == 127U)
            return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}
static UmiStatus PresetGraphField(const UmiJsonTree *tree, int object, const char *key, char *out,
                                  size_t capacity, bool required, bool multiline, bool *present)
{
    int token;
    UmiStatus status = PresetGraphMember(tree, object, key, &token);
    if (present != NULL)
        *present = token >= 0;
    return status == UMI_STATUS_OK ? PresetGraphText(tree, token, out, capacity, required, multiline)
                                   : status;
}
static UmiStatus PresetGraphChoice(PresetGraph *graph, size_t file, int object, UmiProjectPresetStage stage)
{
    if (graph->catalogue->summary.count == UMI_PROJECT_PRESET_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiJsonTree *tree = graph->files[file].tree;
    if (UmiJsonTreeKind(tree, object) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        return UMI_STATUS_PARSE_ERROR;
    size_t index = graph->catalogue->summary.count;
    UmiProjectPresetChoice *choice = &graph->catalogue->choices[index];
    PresetChoiceNode *node = &graph->choices[index];
    node->file = file;
    choice->stage = stage;
    choice->from_user_file = graph->files[file].user;
    UmiStatus status =
        PresetGraphField(tree, object, "name", choice->name, sizeof(choice->name), true, false, NULL);
    if (status == UMI_STATUS_OK)
        status = PresetGraphField(tree, object, "displayName", choice->display_name,
                                  sizeof(choice->display_name), false, false, NULL);
    if (status == UMI_STATUS_OK)
        status = PresetGraphField(tree, object, "description", choice->description,
                                  sizeof(choice->description), false, true, NULL);
    if (status == UMI_STATUS_OK)
        status = PresetGraphField(tree, object, "configurePreset", choice->configure_preset,
                                  sizeof(choice->configure_preset), false, false, &node->configure);
    if (status == UMI_STATUS_OK)
        status = PresetGraphField(tree, object, "binaryDir", choice->binary_directory,
                                  sizeof(choice->binary_directory), false, false, &node->binary);
    if (status != UMI_STATUS_OK)
        return status;
    for (size_t i = 0U; i < index; ++i)
        if (graph->catalogue->choices[i].stage == stage &&
            strcmp(graph->catalogue->choices[i].name, choice->name) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    int token, value;
    status = PresetGraphMember(tree, object, "hidden", &token);
    if (status != UMI_STATUS_OK)
        return status;
    if (token >= 0)
    {
        if (UmiJsonTreeBoolean(tree, token, &value) != UMI_STATUS_OK)
            return UMI_STATUS_PARSE_ERROR;
        choice->hidden = value != 0;
    }
    status = PresetGraphMember(tree, object, "condition", &token);
    if (status != UMI_STATUS_OK)
        return status;
    if (token >= 0)
    {
        node->condition = 1;
        if (UmiJsonTreeIsNull(tree, token))
            node->condition = 2;
        else if (UmiJsonTreeKind(tree, token) == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
            choice->condition_deferred = true;
        else if (UmiJsonTreeBoolean(tree, token, &value) == UMI_STATUS_OK)
            choice->condition_false = value == 0;
        else
            return UMI_STATUS_PARSE_ERROR;
    }
    status = PresetGraphMember(tree, object, "inherits", &node->parents);
    if (status != UMI_STATUS_OK)
        return status;
    if (node->parents >= 0 && UmiJsonTreeKind(tree, node->parents) != UMI_LANGUAGE_RUNTIME_JSON_STRING &&
        UmiJsonTreeKind(tree, node->parents) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        return UMI_STATUS_PARSE_ERROR;
    choice->inherits = node->parents >= 0;
    UmiProjectPresetOrigin *origin = &graph->catalogue->origins[index];
    memcpy(origin->file_path, graph->files[file].path, strlen(graph->files[file].path) + 1U);
    origin->included_file = !graph->files[file].standard;
    ++graph->catalogue->summary.count;
    return UMI_STATUS_OK;
}

/* Expand only filesystem locations whose values come from this read request.
 * Environment, vendor and preset-dependent macros remain CMake's responsibility.
 * Replacements are not recursively expanded, so '$' in a real folder name is
 * preserved instead of interpreted as a second expression. */
static UmiStatus PresetGraphIncludePath(const PresetGraph *graph, const char *file, int64_t schema,
                                        const char *input, char *out)
{
    char parent[UMI_BUILD_PATH_CAPACITY], expanded[UMI_BUILD_PATH_CAPACITY];
    UmiStatus status = umi_path_parent(file, parent, sizeof(parent));
    if (status != UMI_STATUS_OK)
        return status;
    size_t used = 0U;
    for (size_t i = 0U; input[i] != '\0';)
    {
        const char *value = input + i;
        size_t consumed = 1U, length = 1U;
        if (input[i] == '$')
        {
            if (schema < 9)
                return UMI_STATUS_NOT_IMPLEMENTED;
            if (strncmp(input + i, "${sourceDir}", 12U) == 0)
            {
                value = graph->root;
                consumed = 12U;
            }
            else if (strncmp(input + i, "${fileDir}", 10U) == 0)
            {
                value = parent;
                consumed = 10U;
            }
            else
                return UMI_STATUS_NOT_IMPLEMENTED;
            length = strlen(value);
        }
        if (length >= sizeof(expanded) - used)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        memcpy(expanded + used, value, length);
        used += length;
        i += consumed;
    }
    expanded[used] = '\0';
    if (used == 0U)
        return UMI_STATUS_PARSE_ERROR;
    return umi_path_absolute(expanded, parent, out, UMI_BUILD_PATH_CAPACITY);
}
static UmiStatus PresetGraphLoad(PresetGraph *graph, const char *path, bool optional, bool user,
                                 bool standard, size_t depth, int *outIndex)
{
    *outIndex = -1;
    UmiStatus status = PresetGraphCancelled(graph);
    if (status != UMI_STATUS_OK)
        return status;
    memcpy(graph->problem, path, strlen(path) + 1U);
    /* Normalised spellings deduplicate diamond includes. Native aliases that
     * escape lexical comparison still encounter the document/depth bounds. */
    for (size_t i = 0U; i < graph->file_count; ++i)
        if (umi_path_equal(graph->files[i].path, path))
        {
            if (graph->files[i].state == 1)
                return UMI_STATUS_PARSE_ERROR;
            *outIndex = (int)i;
            return UMI_STATUS_OK;
        }
    if (depth >= PRESET_DOCUMENT_CAPACITY || graph->file_count == PRESET_DOCUMENT_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    unsigned char *bytes = NULL;
    size_t length = 0U;
    size_t maximum = PRESET_TOTAL_BYTES - graph->bytes;
    if (maximum > PRESET_DOCUMENT_BYTES)
        maximum = PRESET_DOCUMENT_BYTES;
    status = UmiInputFileRead(path, maximum, &bytes, &length);
    if (optional && status == UMI_STATUS_NOT_FOUND)
        return UMI_STATUS_OK;
    if (status != UMI_STATUS_OK)
        return status;
    size_t index = graph->file_count++;
    PresetDocumentNode *file = &graph->files[index];
    memcpy(file->path, path, strlen(path) + 1U);
    file->user = user;
    file->standard = standard;
    file->state = 1;
    graph->bytes += length;
    /* Accept the optional UTF-8 BOM, then validate the entire JSON document,
     * including fields which this metadata reader does not otherwise inspect. */
    size_t prefix = length >= 3U && bytes[0] == 0xefU && bytes[1] == 0xbbU && bytes[2] == 0xbfU ? 3U : 0U;
    UmiJsonTreeLimits limits = {PRESET_DOCUMENT_BYTES, 262144U, 128U};
    status = UmiJsonTreeCreate(bytes + prefix, length - prefix, &limits, graph->cancel, &file->tree);
    UmiInputFileFree(bytes);
    if (status != UMI_STATUS_OK)
        return status;
    if (UmiJsonTreeKind(file->tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        return UMI_STATUS_PARSE_ERROR;
    int field;
    int64_t schema = 0;
    status = PresetGraphMember(file->tree, 0, "version", &field);
    if (status != UMI_STATUS_OK || UmiJsonTreeInteger(file->tree, field, &schema) != UMI_STATUS_OK ||
        schema < 1 || schema > INT32_MAX)
        return UMI_STATUS_PARSE_ERROR;
    const char *groups[] = {"configurePresets", "buildPresets", "testPresets"};
    for (size_t group = 0U; group < 3U; ++group)
    {
        status = PresetGraphMember(file->tree, 0, groups[group], &field);
        if (status != UMI_STATUS_OK)
            return status;
        if (field < 0)
            continue;
        if (UmiJsonTreeKind(file->tree, field) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            return UMI_STATUS_PARSE_ERROR;
        for (int child = UmiJsonTreeFirst(file->tree, field); child >= 0;
             child = UmiJsonTreeNext(file->tree, child))
        {
            status = PresetGraphCancelled(graph);
            if (status == UMI_STATUS_OK)
                status = PresetGraphChoice(graph, index, child, (UmiProjectPresetStage)group);
            if (status != UMI_STATUS_OK)
                return status;
        }
    }
    status = PresetGraphMember(file->tree, 0, "include", &field);
    if (status != UMI_STATUS_OK)
        return status;
    if (field >= 0)
    {
        if (schema < 4 || UmiJsonTreeKind(file->tree, field) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            return UMI_STATUS_PARSE_ERROR;
        for (int child = UmiJsonTreeFirst(file->tree, field); child >= 0;
             child = UmiJsonTreeNext(file->tree, child))
        {
            char include[UMI_BUILD_PATH_CAPACITY], absolute[UMI_BUILD_PATH_CAPACITY];
            status = PresetGraphText(file->tree, child, include, sizeof(include), true, false);
            if (status == UMI_STATUS_OK)
                status = PresetGraphIncludePath(graph, path, schema, include, absolute);
            if (status != UMI_STATUS_OK)
                return status;
            int next = -1;
            status = PresetGraphLoad(graph, absolute, false, user, false, depth + 1U, &next);
            if (status != UMI_STATUS_OK)
                return status;
            file->visible |= (UINT64_C(1) << (unsigned)next) | graph->files[next].visible;
            memcpy(graph->problem, path, strlen(path) + 1U);
        }
    }
    file->state = 2;
    *outIndex = (int)index;
    return UMI_STATUS_OK;
}
static UmiStatus PresetGraphResolve(PresetGraph *graph, size_t index)
{
    PresetChoiceNode *node = &graph->choices[index];
    if (node->state == 2)
        return UMI_STATUS_OK;
    if (node->state == 1)
        return UMI_STATUS_PARSE_ERROR;
    UmiStatus status = PresetGraphCancelled(graph);
    if (status != UMI_STATUS_OK)
        return status;
    node->state = 1;
    PresetDocumentNode *file = &graph->files[node->file];
    memcpy(graph->problem, file->path, strlen(file->path) + 1U);
    UmiProjectPresetChoice *choice = &graph->catalogue->choices[index];
    bool single = UmiJsonTreeKind(file->tree, node->parents) == UMI_LANGUAGE_RUNTIME_JSON_STRING;
    int parent = single ? node->parents : UmiJsonTreeFirst(file->tree, node->parents);
    while (parent >= 0)
    {
        char name[UMI_BUILD_NAME_CAPACITY];
        status = PresetGraphText(file->tree, parent, name, sizeof(name), true, false);
        if (status != UMI_STATUS_OK)
            return status;
        size_t selected = graph->catalogue->summary.count;
        for (size_t i = 0U; i < graph->catalogue->summary.count; ++i)
            if (graph->catalogue->choices[i].stage == choice->stage &&
                strcmp(graph->catalogue->choices[i].name, name) == 0)
            {
                selected = i;
                break;
            }
        if (selected == graph->catalogue->summary.count)
            return UMI_STATUS_NOT_FOUND;
        PresetChoiceNode *base = &graph->choices[selected];
        if (base->file != node->file && (file->visible & (UINT64_C(1) << base->file)) == 0U)
            return UMI_STATUS_PERMISSION_DENIED;
        status = PresetGraphResolve(graph, selected);
        if (status != UMI_STATUS_OK)
            return status;
        memcpy(graph->problem, file->path, strlen(file->path) + 1U);
        UmiProjectPresetChoice *inherited = &graph->catalogue->choices[selected];
        bool copied = false;
        /* Earlier parents win only for fields they actually provide. Local
         * values, including an empty string or null condition, remain local. */
        if (!node->configure && base->configure)
        {
            memcpy(choice->configure_preset, inherited->configure_preset, sizeof(choice->configure_preset));
            node->configure = true;
            copied = true;
        }
        if (!node->binary && base->binary)
        {
            memcpy(choice->binary_directory, inherited->binary_directory, sizeof(choice->binary_directory));
            node->binary = true;
            copied = true;
        }
        if (node->condition == 0 && base->condition == 1)
        {
            choice->condition_false = inherited->condition_false;
            choice->condition_deferred = inherited->condition_deferred;
            node->condition = 1;
            copied = true;
        }
        graph->catalogue->origins[index].inherited_metadata |= copied;
        parent = single ? -1 : UmiJsonTreeNext(file->tree, parent);
    }
    node->state = 2;
    return UMI_STATUS_OK;
}
UmiStatus UmiProjectPresetCatalogueReadExpanded(const char *projectDirectory,
                                                const UmiCancellationToken *cancel,
                                                UmiProjectPresetCatalogue **out,
                                                UmiProjectPresetReadReport *report)
{
    if (report != NULL)
        memset(report, 0, sizeof(*report));
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (!umi_path_is_absolute(projectDirectory))
        return UMI_STATUS_INVALID_ARGUMENT;
    PresetGraph *graph = calloc(1U, sizeof(*graph));
    UmiProjectPresetCatalogue *catalogue = calloc(1U, sizeof(*catalogue));
    if (graph == NULL || catalogue == NULL)
    {
        free(graph);
        free(catalogue);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    graph->catalogue = catalogue;
    graph->cancel = cancel;
    catalogue->expanded = true;
    UmiStatus status = umi_path_normalise(projectDirectory, graph->root, sizeof(graph->root));
    char path[UMI_BUILD_PATH_CAPACITY];
    int project = -1, user = -1;
    if (status == UMI_STATUS_OK)
        status = umi_path_join(graph->root, "CMakePresets.json", path, sizeof(path));
    if (status == UMI_STATUS_OK)
        status = PresetGraphLoad(graph, path, true, false, true, 0U, &project);
    if (status == UMI_STATUS_OK)
        status = umi_path_join(graph->root, "CMakeUserPresets.json", path, sizeof(path));
    if (status == UMI_STATUS_OK)
        status = PresetGraphLoad(graph, path, true, true, true, 0U, &user);
    if (status == UMI_STATUS_OK && project < 0 && user < 0)
        status = UMI_STATUS_NOT_FOUND;
    if (status == UMI_STATUS_OK && project >= 0 && user >= 0)
    {
        /* CMakeUserPresets implicitly includes CMakePresets. Adding this edge
         * must also reject a reverse explicit include which would make a cycle. */
        if (project == user || (graph->files[project].visible & (UINT64_C(1) << (unsigned)user)) != 0U)
            status = UMI_STATUS_PARSE_ERROR;
        else
            graph->files[user].visible |= (UINT64_C(1) << (unsigned)project) | graph->files[project].visible;
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < catalogue->summary.count; ++i)
        status = PresetGraphResolve(graph, i);
    if (status == UMI_STATUS_OK)
        status = PresetGraphCancelled(graph);
    if (report != NULL)
    {
        report->file_count = graph->file_count;
        report->total_bytes = graph->bytes;
        if (status != UMI_STATUS_OK)
            memcpy(report->problem_file, graph->problem, sizeof(report->problem_file));
    }
    if (status == UMI_STATUS_OK)
    {
        catalogue->summary.project_file = project >= 0;
        catalogue->summary.user_file = user >= 0;
        *out = catalogue;
    }
    else
        free(catalogue);
    for (size_t i = 0U; i < graph->file_count; ++i)
        UmiJsonTreeDestroy(graph->files[i].tree);
    free(graph);
    return status;
}
UmiStatus UmiProjectPresetCatalogueOriginAt(const UmiProjectPresetCatalogue *catalogue, size_t index,
                                            UmiProjectPresetOrigin *out)
{
    if (catalogue == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!catalogue->expanded || index >= catalogue->summary.count)
        return UMI_STATUS_NOT_FOUND;
    *out = catalogue->origins[index];
    return UMI_STATUS_OK;
}
