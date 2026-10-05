/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/preset_catalogue.c
 * PURPOSE: Own bounded preset discovery and keep selection independent from execution.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_project/preset_catalogue.h"
#include "umicom/language_runtime/json_document.h"
#include "umicom/language_runtime/json_text.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/path.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiProjectPresetCatalogue
{
    UmiProjectPresetSummary summary;
    UmiProjectPresetChoice choices[UMI_PROJECT_PRESET_LIMIT];
    /* Expanded discovery records provenance without changing the existing
     * choice or summary layouts used by direct-document clients. */
    bool expanded;
    UmiProjectPresetOrigin origins[UMI_PROJECT_PRESET_LIMIT];
};

/* Decode names before matching so an escaped spelling cannot hide a duplicate
 * field. An unrelated longer key cannot equal any of our short property names.
 * -1 means absent; -2 means malformed or duplicated, never an optional default. */
static int PresetField(const UmiLanguageRuntimeJsonDocument *doc, int object, const char *name)
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
static UmiStatus PresetText(const UmiLanguageRuntimeJsonDocument *doc, int object, const char *field,
                            char *out, size_t capacity, bool required, bool multiline)
{
    int token = PresetField(doc, object, field);
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
/* String lists are inspected only for shape. Includes and inheritance remain
 * descriptive evidence; following them would require a separate path policy. */
static UmiStatus PresetStringList(const UmiLanguageRuntimeJsonDocument *doc, int token, bool single)
{
    if (token == -1)
        return UMI_STATUS_OK;
    if (token < 0)
        return UMI_STATUS_PARSE_ERROR;
    if (single && doc->tokens[token].type == UMI_LANGUAGE_RUNTIME_JSON_STRING)
        return UMI_STATUS_OK;
    if (doc->tokens[token].type != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < umi_language_runtime_json_array_count(doc, token); ++i)
    {
        int child = umi_language_runtime_json_array_at(doc, token, i);
        if (child < 0 || doc->tokens[child].type != UMI_LANGUAGE_RUNTIME_JSON_STRING)
            return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}
static UmiStatus PresetChoice(const UmiLanguageRuntimeJsonDocument *doc, int object,
                              UmiProjectPresetStage stage, bool user, UmiProjectPresetCatalogue *catalogue)
{
    UmiProjectPresetChoice choice = {0};
    choice.stage = stage;
    choice.from_user_file = user;
    UmiStatus status = PresetText(doc, object, "name", choice.name, sizeof(choice.name), true, false);
    if (status == UMI_STATUS_OK)
        status = PresetText(doc, object, "displayName", choice.display_name, sizeof(choice.display_name),
                            false, false);
    if (status == UMI_STATUS_OK)
        status = PresetText(doc, object, "description", choice.description, sizeof(choice.description), false,
                            true);
    if (status == UMI_STATUS_OK)
        status = PresetText(doc, object, "configurePreset", choice.configure_preset,
                            sizeof(choice.configure_preset), false, false);
    if (status == UMI_STATUS_OK)
        status = PresetText(doc, object, "binaryDir", choice.binary_directory,
                            sizeof(choice.binary_directory), false, false);
    if (status != UMI_STATUS_OK)
        return status;
    int hidden = PresetField(doc, object, "hidden"), value = 0;
    if (hidden != -1)
    {
        if (hidden < 0 || umi_language_runtime_json_bool(doc, hidden, &value) != UMI_STATUS_OK)
            return UMI_STATUS_PARSE_ERROR;
        choice.hidden = value != 0;
    }
    int condition = PresetField(doc, object, "condition");
    if (condition < -1)
        return UMI_STATUS_PARSE_ERROR;
    if (condition >= 0 && !umi_language_runtime_json_is_null(doc, condition))
    {
        if (doc->tokens[condition].type == UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
            choice.condition_deferred = true;
        else if (umi_language_runtime_json_bool(doc, condition, &value) == UMI_STATUS_OK)
            choice.condition_false = value == 0;
        else
            return UMI_STATUS_PARSE_ERROR;
    }
    int inherits = PresetField(doc, object, "inherits");
    status = PresetStringList(doc, inherits, true);
    if (status != UMI_STATUS_OK)
        return status;
    choice.inherits = inherits >= 0;
    for (size_t i = 0U; i < catalogue->summary.count; ++i)
        if (catalogue->choices[i].stage == stage && strcmp(catalogue->choices[i].name, choice.name) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    if (catalogue->summary.count == UMI_PROJECT_PRESET_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->choices[catalogue->summary.count++] = choice;
    return UMI_STATUS_OK;
}
static UmiStatus PresetDocument(const void *bytes, size_t size, bool user,
                                UmiProjectPresetCatalogue *catalogue)
{
    if (size == 0U)
        return UMI_STATUS_PARSE_ERROR;
    if (size > UMI_PROJECT_PRESET_FILE_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(bytes, '\0', size) != NULL)
        return UMI_STATUS_PARSE_ERROR;
    const unsigned char *input = bytes;
    if (size >= 3U && input[0] == 0xefU && input[1] == 0xbbU && input[2] == 0xbfU)
    {
        input += 3U;
        size -= 3U;
    }
    char *text = malloc(size + 1U);
    UmiLanguageRuntimeJsonDocument *doc = malloc(sizeof(*doc));
    if (text == NULL || doc == NULL)
    {
        free(text);
        free(doc);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(text, input, size);
    text[size] = '\0';
    UmiStatus status = UmiLanguageRuntimeJsonParseComplete(text, doc);
    if (status == UMI_STATUS_OK)
    {
        int version = PresetField(doc, 0, "version");
        int64_t number = 0;
        if (version < 0 || umi_language_runtime_json_int64(doc, version, &number) != UMI_STATUS_OK ||
            number < 1 || number > INT32_MAX)
            status = UMI_STATUS_PARSE_ERROR;
    }
    if (status == UMI_STATUS_OK)
    {
        int includes = PresetField(doc, 0, "include");
        status = PresetStringList(doc, includes, false);
        if (status == UMI_STATUS_OK && includes >= 0 &&
            umi_language_runtime_json_array_count(doc, includes) != 0U)
            catalogue->summary.includes_not_read = true;
    }
    const char *groups[] = {"configurePresets", "buildPresets", "testPresets"};
    for (size_t group = 0U; status == UMI_STATUS_OK && group < 3U; ++group)
    {
        int array = PresetField(doc, 0, groups[group]);
        if (array == -1)
            continue;
        if (array < 0 || doc->tokens[array].type != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        {
            status = UMI_STATUS_PARSE_ERROR;
            break;
        }
        for (size_t i = 0U; status == UMI_STATUS_OK && i < umi_language_runtime_json_array_count(doc, array);
             ++i)
            status = PresetChoice(doc, umi_language_runtime_json_array_at(doc, array, i),
                                  (UmiProjectPresetStage)group, user, catalogue);
    }
    free(doc);
    free(text);
    return status;
}
UmiStatus UmiProjectPresetCatalogueCreate(const void *projectBytes, size_t projectSize, const void *userBytes,
                                          size_t userSize, UmiProjectPresetCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if ((projectBytes == NULL && projectSize != 0U) || (userBytes == NULL && userSize != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (projectBytes == NULL && userBytes == NULL)
        return UMI_STATUS_NOT_FOUND;
    UmiProjectPresetCatalogue *catalogue = calloc(1U, sizeof(*catalogue));
    if (catalogue == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    catalogue->summary.project_file = projectBytes != NULL;
    catalogue->summary.user_file = userBytes != NULL;
    UmiStatus status =
        projectBytes != NULL ? PresetDocument(projectBytes, projectSize, false, catalogue) : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK && userBytes != NULL)
        status = PresetDocument(userBytes, userSize, true, catalogue);
    if (status == UMI_STATUS_OK)
        *out = catalogue;
    else
        free(catalogue);
    return status;
}
UmiStatus UmiProjectPresetCatalogueRead(const char *projectDirectory, UmiProjectPresetCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (!umi_path_is_absolute(projectDirectory))
        return UMI_STATUS_INVALID_ARGUMENT;
    unsigned char *bytes[2] = {NULL, NULL};
    size_t sizes[2] = {0U, 0U};
    const char *names[] = {"CMakePresets.json", "CMakeUserPresets.json"};
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < 2U; ++i)
    {
        char path[UMI_BUILD_PATH_CAPACITY];
        status = umi_path_join(projectDirectory, names[i], path, sizeof(path));
        if (status == UMI_STATUS_OK)
            status = UmiInputFileRead(path, UMI_PROJECT_PRESET_FILE_LIMIT, &bytes[i], &sizes[i]);
        if (status == UMI_STATUS_NOT_FOUND)
            status = UMI_STATUS_OK;
    }
    if (status == UMI_STATUS_OK)
        status = UmiProjectPresetCatalogueCreate(bytes[0], sizes[0], bytes[1], sizes[1], out);
    UmiInputFileFree(bytes[0]);
    UmiInputFileFree(bytes[1]);
    return status;
}
void UmiProjectPresetCatalogueDestroy(UmiProjectPresetCatalogue *catalogue) { free(catalogue); }
UmiStatus UmiProjectPresetCatalogueSummary(const UmiProjectPresetCatalogue *catalogue,
                                           UmiProjectPresetSummary *out)
{
    if (catalogue == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = catalogue->summary;
    return UMI_STATUS_OK;
}
UmiStatus UmiProjectPresetCatalogueAt(const UmiProjectPresetCatalogue *catalogue, size_t index,
                                      UmiProjectPresetChoice *out)
{
    if (catalogue == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= catalogue->summary.count)
        return UMI_STATUS_NOT_FOUND;
    *out = catalogue->choices[index];
    return UMI_STATUS_OK;
}
UmiStatus UmiProjectPresetCatalogueSelect(const UmiProjectPresetCatalogue *catalogue, size_t index,
                                          const UmiBuildProfile *profile, UmiBuildProfile *out)
{
    if (catalogue == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_build_profile_validate(profile, NULL, 0U);
    if (status != UMI_STATUS_OK)
        return status;
    if (index >= catalogue->summary.count)
        return UMI_STATUS_NOT_FOUND;
    if (profile->preset[0] != '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    const UmiProjectPresetChoice *choice = &catalogue->choices[index];
    if (choice->hidden || choice->condition_false)
        return UMI_STATUS_PERMISSION_DENIED;
    UmiBuildProfile candidate = *profile;
    char *destination = choice->stage == UMI_PROJECT_PRESET_CONFIGURE ? candidate.configure_preset
                        : choice->stage == UMI_PROJECT_PRESET_BUILD   ? candidate.build_preset
                                                                      : candidate.test_preset;
    memcpy(destination, choice->name, strlen(choice->name) + 1U);
    *out = candidate;
    return UMI_STATUS_OK;
}

/* Included-file traversal and inherited metadata share the catalogue owner. */
#include "preset_graph.inc"
