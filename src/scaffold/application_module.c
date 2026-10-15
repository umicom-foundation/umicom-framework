/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/scaffold/application_module.c
 * PURPOSE: Render bounded application templates and publish reviewed source without replacing files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/scaffold/application_module.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/workspace_files.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The catalogue is code-owned: templates cannot choose output paths or add
 * executable hooks. Extend it together with the generated-project regressions. */
static const char *const module_files[] = {
    "CMakeLists.txt", "application.umicom.yaml", "README.md", ".gitignore",
    "src/model.h", "src/model.c", "src/console.c", "src/gtk.c",
    "tests/test_model.c", "docs/application-features.md", "LICENSE"
};
static const char *const module_directories[] = {"src", "tests", "docs"};
_Static_assert(sizeof(module_files) / sizeof(module_files[0]) <=
    UMI_APPLICATION_SCAFFOLD_FILE_LIMIT, "Scaffold catalogue exceeds its bound");

struct UmiApplicationScaffoldPlan {
    UmiApplicationScaffoldSummary summary;
    char parent[UMI_PATH_CAPACITY];
    char slug[64];
    char *texts[UMI_APPLICATION_SCAFFOLD_FILE_LIMIT];
    size_t lengths[UMI_APPLICATION_SCAFFOLD_FILE_LIMIT];
};

/* ASCII identifiers avoid locale-dependent spelling and template injection.
 * A slug starts with a letter, contains lower-case letters, digits or hyphens,
 * and ends with a letter or digit. The path service also rejects device names. */
static int module_letter(char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}
static int module_digit(char ch) { return ch >= '0' && ch <= '9'; }
/* The local slug checker is retained for review. The shared runtime contract
 * now keeps creation, inventory and registration preflight in agreement. */
#if 0
static int module_slug(const char *text)
{
    size_t length;
    if (text == NULL) return 0;
    length = strlen(text);
    if (length == 0U || length >= 64U || text[0] < 'a' || text[0] > 'z' ||
        text[length - 1U] == '-') return 0;
    for (size_t i = 0U; i < length; ++i)
        if (!(text[i] >= 'a' && text[i] <= 'z') &&
            !module_digit(text[i]) && text[i] != '-') return 0;
    /* Refuse Windows device aliases on every host so a project created on
     * Linux can later be checked out on Windows without renaming its folder. */
    if (strcmp(text, "con") == 0 || strcmp(text, "prn") == 0 ||
        strcmp(text, "aux") == 0 || strcmp(text, "nul") == 0 ||
        (length == 4U && (memcmp(text, "com", 3U) == 0 ||
         memcmp(text, "lpt", 3U) == 0) && text[3] >= '1' && text[3] <= '9'))
        return 0;
    return 1;
}
#endif
static int module_slug(const char *text)
{
    return umi_application_module_slug_validate(text) == UMI_STATUS_OK;
}
static int module_name(const char *text)
{
    size_t length;
    if (text == NULL) return 0;
    length = strlen(text);
    if (length == 0U || length > 120U || !module_letter(text[0]) ||
        text[length - 1U] == ' ') return 0;
    for (size_t i = 0U; i < length; ++i)
        if (!module_letter(text[i]) && !module_digit(text[i]) &&
            text[i] != ' ' && text[i] != '-') return 0;
    return 1;
}
static int module_id(const char *text)
{
    size_t length;
    bool segment_start = true, dotted = false;
    if (text == NULL) return 0;
    length = strlen(text);
    if (length == 0U || length > 200U) return 0;
    /* GTK application IDs consist of nonempty dotted identifiers. Validate
     * that form even for console projects so adding a GUI preserves identity. */
    for (size_t i = 0U; i < length; ++i) {
        if (text[i] == '.') {
            if (segment_start) return 0;
            segment_start = true;
            dotted = true;
        } else {
            if (segment_start && !module_letter(text[i])) return 0;
            if (!module_letter(text[i]) && !module_digit(text[i]) &&
                text[i] != '-') return 0;
            segment_start = false;
        }
    }
    return dotted && !segment_start;
}

/* Absolute normalised paths make the preview independent of a later working
 * directory change. Reject relative syntax instead of silently choosing a home
 * or cache folder for the user's new source. */
static UmiStatus module_absolute(const char *path)
{
    char normal[UMI_PATH_CAPACITY];
    UmiStatus status = UmiOutputFileValidatePath(path);
    if (status != UMI_STATUS_OK) return status;
    status = umi_path_normalise(path, normal, sizeof(normal));
    if (status != UMI_STATUS_OK) return status;
    return umi_path_equal(path, normal) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* Only recognised tokens are expanded. A template typo fails the preview
 * instead of becoming unexplained text in a generated project. All template
 * bytes are held by the plan before a destination directory is created. */
static UmiStatus module_render(const unsigned char *source, size_t length,
    const char *const *tokens, const char *const *values, size_t token_count,
    char **out_text, size_t *out_length)
{
    char *output = malloc(UMI_APPLICATION_SCAFFOLD_TEXT_LIMIT + 1U);
    size_t used = 0U;
    UmiStatus status = UMI_STATUS_OK;
    if (output == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (size_t pos = 0U; pos < length;) {
        const char *replacement = NULL;
        size_t count = 1U;
        if (source[pos] == 0U) { status = UMI_STATUS_PARSE_ERROR; break; }
        if (source[pos] == '@') {
            for (size_t item = 0U; item < token_count; ++item) {
                size_t token_length = strlen(tokens[item]);
                if (token_length <= length - pos &&
                    memcmp(source + pos, tokens[item], token_length) == 0) {
                    replacement = values[item];
                    count = token_length;
                    break;
                }
            }
            if (replacement == NULL) { status = UMI_STATUS_PARSE_ERROR; break; }
        }
        size_t addition = replacement != NULL ? strlen(replacement) : 1U;
        if (addition > UMI_APPLICATION_SCAFFOLD_TEXT_LIMIT - used) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        memcpy(output + used, replacement != NULL ?
            (const void *)replacement : (const void *)(source + pos), addition);
        used += addition;
        pos += count;
    }
    if (status != UMI_STATUS_OK) { free(output); return status; }
    output[used] = '\0';
    *out_text = output;
    *out_length = used;
    return UMI_STATUS_OK;
}

void umi_application_scaffold_destroy(UmiApplicationScaffoldPlan *plan)
{
    if (plan == NULL) return;
    for (size_t i = 0U; i < UMI_APPLICATION_SCAFFOLD_FILE_LIMIT; ++i)
        free(plan->texts[i]);
    free(plan);
}

UmiStatus umi_application_scaffold_plan(
    const UmiApplicationScaffoldRequest *request,
    UmiApplicationScaffoldPlan **out_plan)
{
    UmiApplicationScaffoldPlan *plan;
    char identifier[64], native_field[160], console_executable[96];
    char path[UMI_PATH_CAPACITY], relative[128];
    const char *const tokens[] = {"@NAME@", "@SLUG@", "@IDENTIFIER@", "@ID@",
        "@GTK_DEFAULT@", "@NATIVE_FIELD@", "@GTK_FRONTEND@", "@CONSOLE_EXECUTABLE@"};
    const char *values[sizeof(tokens) / sizeof(tokens[0])];
    UmiStatus status;
    if (out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_plan = NULL;
    if (request == NULL || !module_slug(request->slug) ||
        !module_name(request->application_name) || !module_id(request->application_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = module_absolute(request->destination_parent);
    if (status == UMI_STATUS_OK) status = module_absolute(request->template_root);
    if (status != UMI_STATUS_OK) return status;
    if (!umi_fs_is_directory(request->destination_parent) ||
        !umi_fs_is_directory(request->template_root)) return UMI_STATUS_NOT_FOUND;
    plan = calloc(1U, sizeof(*plan));
    if (plan == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(plan->parent, request->destination_parent, strlen(request->destination_parent) + 1U);
    memcpy(plan->slug, request->slug, strlen(request->slug) + 1U);
    status = umi_path_join(plan->parent, plan->slug, plan->summary.root,
        sizeof(plan->summary.root));
    if (status == UMI_STATUS_OK) status = module_absolute(plan->summary.root);
    if (status != UMI_STATUS_OK) goto failed;
    if (umi_fs_exists(plan->summary.root)) { status = UMI_STATUS_ALREADY_EXISTS; goto failed; }

    /* C identifiers and manifest declarations are derived once, so the preview,
     * CMake target names, generated includes and discovery option stay aligned. */
    for (size_t i = 0U; i <= strlen(plan->slug); ++i)
        identifier[i] = plan->slug[i] == '-' ? '_' : plan->slug[i];
    native_field[0] = '\0';
    if (request->gtk_frontend)
        (void)snprintf(native_field, sizeof(native_field),
            "  native_executable: umicom-%s\n", plan->slug);
    values[0] = request->application_name; values[1] = plan->slug;
    values[2] = identifier; values[3] = request->application_id;
    values[4] = request->gtk_frontend ? "ON" : "OFF";
    values[5] = native_field;
    values[6] = request->gtk_frontend ? "  - gtk4\n" : "";
    (void)snprintf(console_executable, sizeof(console_executable), "umicom-%s%s",
        plan->slug, request->gtk_frontend ? "-console" : "");
    values[7] = console_executable;
    plan->summary.file_count = sizeof(module_files) / sizeof(module_files[0]);
    for (size_t i = 0U; i < plan->summary.file_count; ++i) {
        unsigned char *bytes = NULL;
        size_t length = 0U;
        /* Validate every destination while still in preview. A root that fits
         * does not prove its longer descendant filenames fit the path bound. */
        status = umi_path_join(plan->summary.root, module_files[i], path, sizeof(path));
        if (status == UMI_STATUS_OK) status = UmiOutputFileValidatePath(path);
        if (status != UMI_STATUS_OK) goto failed;
        (void)snprintf(relative, sizeof(relative), "%s.in", module_files[i]);
        status = umi_path_join(request->template_root, relative, path, sizeof(path));
        if (status == UMI_STATUS_OK)
            status = UmiInputFileRead(path, UMI_APPLICATION_SCAFFOLD_TEXT_LIMIT,
                &bytes, &length);
        if (status == UMI_STATUS_OK)
            status = module_render(bytes, length, tokens, values,
                sizeof(tokens) / sizeof(tokens[0]), &plan->texts[i], &plan->lengths[i]);
        UmiInputFileFree(bytes);
        if (status != UMI_STATUS_OK) goto failed;
        plan->summary.byte_count += plan->lengths[i];
    }
    /* Reuse the real descriptor parser, not a parallel validator that might
     * accept a project the suite cannot later discover. */
    status = umi_application_module_parse(plan->texts[1], plan->lengths[1],
        &plan->summary.module);
    if (status != UMI_STATUS_OK) goto failed;
    /* A customised template must still describe the requested application.
     * Otherwise a preview could register a different identity or executable. */
    if (strcmp(plan->summary.module.id, request->application_id) != 0 ||
        strcmp(plan->summary.module.name, request->application_name) != 0 ||
        strcmp(plan->summary.module.console_executable, console_executable) != 0) {
        status = UMI_STATUS_INVALID_STATE;
        goto failed;
    }
    (void)snprintf(relative, sizeof(relative), "umicom-%s", plan->slug);
    if (strcmp(plan->summary.module.executable, relative) != 0 ||
        (request->gtk_frontend && strcmp(plan->summary.module.native_executable, relative) != 0) ||
        (!request->gtk_frontend && plan->summary.module.native_executable[0] != '\0')) {
        status = UMI_STATUS_INVALID_STATE;
        goto failed;
    }
    *out_plan = plan;
    return UMI_STATUS_OK;
failed:
    umi_application_scaffold_destroy(plan);
    return status;
}

UmiStatus umi_application_scaffold_summary(const UmiApplicationScaffoldPlan *plan,
    UmiApplicationScaffoldSummary *out_summary)
{
    if (out_summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_summary, 0, sizeof(*out_summary));
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_summary = plan->summary;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_scaffold_file(const UmiApplicationScaffoldPlan *plan,
    size_t index, UmiApplicationScaffoldFile *out_file)
{
    if (out_file == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_file, 0, sizeof(*out_file));
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= plan->summary.file_count) return UMI_STATUS_NOT_FOUND;
    out_file->relative_path = module_files[index];
    out_file->text = plan->texts[index];
    out_file->length = plan->lengths[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_application_scaffold_apply(UmiApplicationScaffoldPlan *plan,
    UmiApplicationScaffoldResult *out_result)
{
    UmiWorkspaceGraph *graph = NULL;
    UmiWorkspaceEntryResult entry;
    char relative[UMI_PATH_CAPACITY];
    UmiStatus status;
    if (out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_result, 0, sizeof(*out_result));
    if (plan == NULL) { out_result->status = UMI_STATUS_INVALID_ARGUMENT; return out_result->status; }
    if (plan->summary.apply_started) {
        out_result->status = UMI_STATUS_INVALID_STATE;
        return out_result->status;
    }
    plan->summary.apply_started = true;
    /* The workspace service owns exclusive directory creation and portable
     * names. Its trust flag reflects the caller's explicit apply decision. */
    status = umi_workspace_graph_create(&graph);
    if (status == UMI_STATUS_OK) status = umi_workspace_graph_open(graph, plan->parent, 1);
    (void)snprintf(out_result->last_path, sizeof(out_result->last_path), "%s", plan->summary.root);
    if (status == UMI_STATUS_OK) {
        status = UmiWorkspaceCreateEntry(graph, 0U, plan->slug,
            UMI_WORKSPACE_ENTRY_DIRECTORY, &entry);
        if (entry.created) ++out_result->created_directories;
    }
    for (size_t i = 0U; status == UMI_STATUS_OK &&
        i < sizeof(module_directories) / sizeof(module_directories[0]); ++i) {
        status = umi_path_join(plan->slug, module_directories[i], relative, sizeof(relative));
        if (status == UMI_STATUS_OK)
            status = umi_path_join(plan->parent, relative, out_result->last_path,
                sizeof(out_result->last_path));
        if (status == UMI_STATUS_OK) {
            status = UmiWorkspaceCreateEntry(graph, 0U, relative,
                UMI_WORKSPACE_ENTRY_DIRECTORY, &entry);
            if (entry.created) ++out_result->created_directories;
        }
    }
    /* Content is written on the exclusive file handle. Unlike create-then-open,
     * this never reopens a leaf another writer could have substituted. The
     * selected parents remain caller-owned, not a filesystem security boundary. */
    for (size_t i = 0U; status == UMI_STATUS_OK && i < plan->summary.file_count; ++i) {
        UmiOutputFile *file = NULL;
        status = umi_path_join(plan->summary.root, module_files[i],
            out_result->last_path, sizeof(out_result->last_path));
        if (status == UMI_STATUS_OK) status = UmiOutputFileCreate(out_result->last_path, &file);
        if (status == UMI_STATUS_OK) {
            UmiStatus close_status;
            ++out_result->created_files;
            status = UmiOutputFileWrite(file, plan->texts[i], plan->lengths[i]);
            close_status = UmiOutputFileClose(file);
            if (status == UMI_STATUS_OK) status = close_status;
            if (status == UMI_STATUS_OK) ++out_result->completed_files;
        }
        UmiOutputFileDestroy(file);
    }
    umi_workspace_graph_destroy(graph);
    out_result->status = status;
    return status;
}
