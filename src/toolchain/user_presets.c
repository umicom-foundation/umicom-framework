/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/user_presets.c
 *
 * PURPOSE:
 *   Generate literal CMake presets through shared JSON emission and exclusive file creation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/toolchain/user_presets.h"
#include "umicom/toolchain/build.h"
#include "umicom/base/utf8.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/output_file.h"
#include "umicom/protocol/json_writer.h"
#include <stdlib.h>
#include <string.h>

static UmiStatus PresetText(const char *text, size_t capacity, int required)
{
    const char *end = memchr(text, '\0', capacity);
    if (end == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
    if ((required && end == text) ||
        !UmiUtf8Validate((const unsigned char *)text, (size_t)(end - text), NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
static UmiStatus PresetTool(const UmiToolchainProfile *profile, UmiToolKind kind,
    const UmiToolInfo **out)
{
    const UmiToolInfo *tool = umi_toolchain_profile_tool(profile, kind);
    if (tool == NULL || tool->state != UMI_TOOL_VALIDATED) return UMI_STATUS_NOT_FOUND;
    if (tool->kind != kind) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = PresetText(tool->path, sizeof tool->path, 1);
    if (status == UMI_STATUS_OK) *out = tool;
    return status;
}
/* JSON escaping and CMake macro expansion are different layers. Convert each
 * literal dollar before JSON encoding, so a discovered path cannot become an
 * environment/sourceDir/vendor macro when CMake later reads the preset. */
static void PresetLiteral(UmiJsonWriter *writer, const char *text)
{
    char literal[UMI_TOOL_PATH_CAPACITY * 9U + 1U];
    size_t used = 0U;
    for (size_t i = 0U; text[i] != '\0'; ++i) {
        if (text[i] == '$') {
            memcpy(literal + used, "${dollar}", 9U);
            used += 9U;
        } else literal[used++] = text[i];
    }
    literal[used] = '\0';
    (void)UmiJsonWriterString(writer, literal);
}
static UmiStatus PresetsRender(const UmiToolchainProfile *profile, char *text, size_t *size)
{
    if (!umi_toolchain_profile_storage_compatible(profile)) return UMI_STATUS_INVALID_ARGUMENT;
    if (profile->selected_c_compiler != UMI_TOOL_GCC &&
        profile->selected_c_compiler != UMI_TOOL_CLANG &&
        profile->selected_c_compiler != UMI_TOOL_MSVC_CL) return UMI_STATUS_INVALID_ARGUMENT;
    const UmiToolInfo *compiler = NULL, *ninja = NULL, *pkg = NULL;
    UmiStatus status = PresetTool(profile, profile->selected_c_compiler, &compiler);
    if (status == UMI_STATUS_OK) status = PresetTool(profile, UMI_TOOL_NINJA, &ninja);
    if (status == UMI_STATUS_OK) status = PresetTool(profile, UMI_TOOL_PKG_CONFIG, &pkg);
    if (status == UMI_STATUS_OK) status = PresetText(profile->prefix_directory, sizeof profile->prefix_directory, 0);
    if (status != UMI_STATUS_OK) return status;
    UmiJsonWriter writer;
    UmiJsonWriterInit(&writer, text, UMI_BUILD_USER_PRESETS_CAPACITY);
    (void)UmiJsonWriterRaw(&writer,
        "{\n  \"version\":6,\n  \"configurePresets\":[{\n"
        "    \"name\":\"umicom-auto-debug\",\n"
        "    \"displayName\":\"Umicom auto-discovered debug\",\n"
        "    \"generator\":\"Ninja\",\n"
        "    \"binaryDir\":\"${sourceDir}/build/umicom-auto-debug\",\n"
        "    \"cacheVariables\":{\n      \"CMAKE_BUILD_TYPE\":\"Debug\",\n"
        "      \"CMAKE_C_COMPILER\":");
    PresetLiteral(&writer, compiler->path);
    (void)UmiJsonWriterRaw(&writer, ",\n      \"CMAKE_MAKE_PROGRAM\":");
    PresetLiteral(&writer, ninja->path);
    (void)UmiJsonWriterRaw(&writer, ",\n      \"PKG_CONFIG_EXECUTABLE\":");
    PresetLiteral(&writer, pkg->path);
    (void)UmiJsonWriterRaw(&writer, ",\n      \"CMAKE_PREFIX_PATH\":");
    PresetLiteral(&writer, profile->prefix_directory);
    (void)UmiJsonWriterRaw(&writer,
        ",\n      \"BUILD_TESTING\":\"ON\"\n    }\n  }],\n"
        "  \"buildPresets\":[{\"name\":\"umicom-auto-debug\",\"configurePreset\":\"umicom-auto-debug\"}],\n"
        "  \"testPresets\":[{\"name\":\"umicom-auto-debug\",\"configurePreset\":\"umicom-auto-debug\","
        "\"output\":{\"outputOnFailure\":true}}]\n}\n");
    if (writer.status == UMI_STATUS_OK) *size = writer.length;
    return writer.status;
}
UmiStatus UmiBuildUserPresetsFormat(const UmiToolchainProfile *profile,
    char *out_json, size_t capacity, size_t *out_size)
{
    if (out_json == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    char *text = malloc(UMI_BUILD_USER_PRESETS_CAPACITY);
    if (text == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    size_t size = 0U;
    UmiStatus status = PresetsRender(profile, text, &size);
    if (status == UMI_STATUS_OK && size >= capacity) status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK) {
        memcpy(out_json, text, size + 1U);
        if (out_size != NULL) *out_size = size;
    }
    free(text);
    return status;
}
UmiStatus UmiBuildUserPresetsCreate(const UmiToolchainProfile *profile,
    const char *project_root, const char *path, int dry_run)
{
    if (dry_run != 0 && dry_run != 1) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiOutputFileValidatePath(project_root);
    if (status == UMI_STATUS_OK) status = UmiOutputFileValidatePath(path);
    if (status == UMI_STATUS_OK && (!umi_path_is_within(project_root, path) || umi_path_equal(project_root, path)))
        status = UMI_STATUS_INVALID_ARGUMENT;
    char parent[UMI_PATH_CAPACITY];
    if (status == UMI_STATUS_OK) status = umi_path_parent(path, parent, sizeof parent);
    if (status == UMI_STATUS_OK && !umi_fs_is_directory(parent)) status = UMI_STATUS_NOT_FOUND;
    if (status != UMI_STATUS_OK) return status;
    char *text = malloc(UMI_BUILD_USER_PRESETS_CAPACITY);
    if (text == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    size_t size = 0U;
    status = PresetsRender(profile, text, &size);
    /* Preview cannot reserve a name. Apply repeats the observation, then the
     * exclusive native create protects against a destination created later. */
    if (status == UMI_STATUS_OK && umi_fs_exists(path)) status = UMI_STATUS_ALREADY_EXISTS;
    UmiOutputFile *file = NULL;
    if (status == UMI_STATUS_OK && !dry_run) {
        status = UmiOutputFileCreate(path, &file);
        if (status == UMI_STATUS_OK) status = UmiOutputFileWrite(file, text, size);
        if (file != NULL) {
            UmiStatus closed = UmiOutputFileClose(file);
            if (status == UMI_STATUS_OK) status = closed;
            UmiOutputFileDestroy(file);
        }
    }
    free(text);
    return status;
}
/* Compatibility callers explicitly retain the previous replace-file policy.
 * Serialization is shared with the create-only API; the CLI no longer uses
 * this replacement entry point. A write can leave partial output on failure. */
UmiStatus umi_build_write_user_presets(const UmiToolchainProfile *profile,
    const char *project_root, const char *path)
{
    if (profile == NULL || project_root == NULL || path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    char *text = malloc(UMI_BUILD_USER_PRESETS_CAPACITY);
    if (text == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    size_t size = 0U;
    UmiStatus status = PresetsRender(profile, text, &size);
    if (status == UMI_STATUS_OK) status = umi_fs_write_text(path, text);
    free(text);
    return status;
}
