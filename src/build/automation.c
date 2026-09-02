/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/automation.c
 *
 * PURPOSE:
 *   Implement changed-file classification, product-scope matching and
 *   conservative transitive planning for the Umicom Automated Build System.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/automation.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * The implementation owns fixed-capacity arrays on the heap. Fixed capacities
 * make memory use predictable and every insertion checks its bound before it
 * writes, while heap allocation keeps large path collections off the stack.
 */
struct UmiBuildAutomation {
    UmiBuildAutomationScope scopes[UMI_BUILD_AUTOMATION_MAX_SCOPES];
    UmiBuildAutomationChange changes[UMI_BUILD_AUTOMATION_MAX_CHANGES];
    UmiBuildAutomationPlanItem items[UMI_BUILD_AUTOMATION_MAX_SCOPES];
    UmiBuildAutomationSnapshot snapshot;
    size_t scope_count;
    size_t change_count;
    size_t item_count;
    uint64_t revision;
};

/* Copy text only when it fits, preventing truncated identifiers from colliding. */
static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    const size_t length = source != NULL ? strlen(source) : 0U;

    if (destination == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (length >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (length > 0U) {
        (void)memcpy(destination, source, length);
    }
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Convert Windows separators to the workspace's portable forward-slash form,
 * remove a leading "./", collapse repeated separators and trim a final slash.
 */
static UmiStatus normalise_path(const char *path,
                                char *out_path,
                                size_t capacity)
{
    size_t input_index = 0U;
    size_t output_index = 0U;
    int previous_was_separator = 0;

    if (path == NULL || out_path == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (path[0] == '.' && (path[1] == '/' || path[1] == '\\')) {
        input_index = 2U;
    }
    while (path[input_index] != '\0') {
        char value = path[input_index++];
        const int is_separator = value == '/' || value == '\\';

        if (is_separator) {
            value = '/';
            if (previous_was_separator) {
                continue;
            }
        }
        if (output_index + 1U >= capacity) {
            out_path[0] = '\0';
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        out_path[output_index++] = value;
        previous_was_separator = is_separator;
    }
    if (output_index > 0U && out_path[output_index - 1U] == '/') {
        output_index -= 1U;
    }
    out_path[output_index] = '\0';
    return UMI_STATUS_OK;
}

/* Compare one ASCII file name without depending on the current locale. */
static int text_equals_case_insensitive(const char *left, const char *right)
{
    size_t index = 0U;

    if (left == NULL || right == NULL) {
        return 0;
    }
    while (left[index] != '\0' && right[index] != '\0') {
        const unsigned char left_value = (unsigned char)left[index];
        const unsigned char right_value = (unsigned char)right[index];

        if (tolower(left_value) != tolower(right_value)) {
            return 0;
        }
        index += 1U;
    }
    return left[index] == '\0' && right[index] == '\0';
}

/* Return the final path component so classification ignores parent names. */
static const char *file_name_from_path(const char *path)
{
    const char *name = path;
    const char *cursor;

    if (path == NULL) {
        return "";
    }
    for (cursor = path; *cursor != '\0'; ++cursor) {
        if (*cursor == '/' || *cursor == '\\') {
            name = cursor + 1;
        }
    }
    return name;
}

/* Match a file extension using a case-insensitive comparison. */
static int path_has_extension(const char *path, const char *extension)
{
    const size_t path_length = path != NULL ? strlen(path) : 0U;
    const size_t extension_length =
        extension != NULL ? strlen(extension) : 0U;

    if (extension_length == 0U || extension_length > path_length) {
        return 0;
    }
    return text_equals_case_insensitive(
        path + path_length - extension_length, extension);
}

/* Check a complete path segment, avoiding accidental partial-name matches. */
static int path_has_segment(const char *path, const char *segment)
{
    const size_t segment_length = segment != NULL ? strlen(segment) : 0U;
    const char *cursor = path;

    if (path == NULL || segment_length == 0U) {
        return 0;
    }
    while ((cursor = strstr(cursor, segment)) != NULL) {
        const int begins_segment = cursor == path || cursor[-1] == '/';
        const int ends_segment = cursor[segment_length] == '\0' ||
                                 cursor[segment_length] == '/';

        if (begins_segment && ends_segment) {
            return 1;
        }
        cursor += 1;
    }
    return 0;
}

/*
 * A prefix must finish on a path boundary. This stops a scope named "media"
 * from accidentally owning a neighbouring path named "media-tools".
 */
static int scope_matches_path(const UmiBuildAutomationScope *scope,
                              const char *path)
{
    const size_t prefix_length = strlen(scope->source_prefix);

    if (prefix_length == 0U) {
        return 1;
    }
    if (strncmp(scope->source_prefix, path, prefix_length) != 0) {
        return 0;
    }
    return path[prefix_length] == '\0' || path[prefix_length] == '/';
}

/* Find the most specific enabled scope for a changed path. */
static const UmiBuildAutomationScope *find_scope_for_path(
    const UmiBuildAutomation *automation,
    const char *path)
{
    const UmiBuildAutomationScope *best = NULL;
    size_t best_length = 0U;
    size_t index;

    for (index = 0U; index < automation->scope_count; ++index) {
        const UmiBuildAutomationScope *scope = &automation->scopes[index];
        const size_t prefix_length = strlen(scope->source_prefix);

        if (!scope->enabled || !scope_matches_path(scope, path)) {
            continue;
        }
        if (best == NULL || prefix_length > best_length) {
            best = scope;
            best_length = prefix_length;
        }
    }
    return best;
}

/* Find a registered scope by its stable identifier. */
static size_t find_scope_index(const UmiBuildAutomation *automation,
                               const char *scope_id)
{
    size_t index;

    for (index = 0U; index < automation->scope_count; ++index) {
        if (strcmp(automation->scopes[index].scope_id, scope_id) == 0) {
            return index;
        }
    }
    return automation->scope_count;
}

/* Find an existing plan item so several paths can be merged safely. */
static size_t find_item_index(const UmiBuildAutomation *automation,
                              const char *scope_id)
{
    size_t index;

    for (index = 0U; index < automation->item_count; ++index) {
        if (strcmp(automation->items[index].scope_id, scope_id) == 0) {
            return index;
        }
    }
    return automation->item_count;
}

/* Map a classified change to the phases that can be affected by that file. */
static UmiBuildAutomationActionFlags actions_for_change(
    UmiBuildAutomationChangeKind kind,
    UmiBuildAutomationActionFlags fallback)
{
    switch (kind) {
        case UMI_BUILD_AUTOMATION_CHANGE_BUILD_DEFINITION:
            return UMI_BUILD_AUTOMATION_ACTION_CONFIGURE |
                   UMI_BUILD_AUTOMATION_ACTION_BUILD |
                   UMI_BUILD_AUTOMATION_ACTION_TEST;
        case UMI_BUILD_AUTOMATION_CHANGE_SOURCE:
        case UMI_BUILD_AUTOMATION_CHANGE_PUBLIC_HEADER:
        case UMI_BUILD_AUTOMATION_CHANGE_PRIVATE_HEADER:
        case UMI_BUILD_AUTOMATION_CHANGE_ASSEMBLY:
        case UMI_BUILD_AUTOMATION_CHANGE_TEST:
        case UMI_BUILD_AUTOMATION_CHANGE_GENERATED_INPUT:
            return UMI_BUILD_AUTOMATION_ACTION_BUILD |
                   UMI_BUILD_AUTOMATION_ACTION_TEST;
        case UMI_BUILD_AUTOMATION_CHANGE_RESOURCE:
            return UMI_BUILD_AUTOMATION_ACTION_BUILD |
                   UMI_BUILD_AUTOMATION_ACTION_STAGE;
        case UMI_BUILD_AUTOMATION_CHANGE_DOCUMENTATION:
            return UMI_BUILD_AUTOMATION_ACTION_DOCUMENT;
        case UMI_BUILD_AUTOMATION_CHANGE_MANIFEST:
            return UMI_BUILD_AUTOMATION_ACTION_CONFIGURE |
                   UMI_BUILD_AUTOMATION_ACTION_BUILD |
                   UMI_BUILD_AUTOMATION_ACTION_TEST |
                   UMI_BUILD_AUTOMATION_ACTION_STAGE;
        case UMI_BUILD_AUTOMATION_CHANGE_UNKNOWN:
        default:
            return fallback;
    }
}

/* Copy scope execution names into a newly selected plan item. */
static void initialise_item(UmiBuildAutomationPlanItem *item,
                            const UmiBuildAutomationScope *scope)
{
    (void)memset(item, 0, sizeof(*item));
    item->structure_size = (uint32_t)sizeof(*item);
    item->api_version = UMI_BUILD_AUTOMATION_API_VERSION;
    (void)copy_text(item->scope_id, sizeof(item->scope_id), scope->scope_id);
    (void)copy_text(item->display_name,
                    sizeof(item->display_name),
                    scope->display_name);
    (void)copy_text(item->version,
                    sizeof(item->version),
                    scope->version);
    (void)copy_text(item->build_target,
                    sizeof(item->build_target),
                    scope->build_target);
    (void)copy_text(item->test_build_target,
                    sizeof(item->test_build_target),
                    scope->test_build_target);
    (void)copy_text(item->test_expression,
                    sizeof(item->test_expression),
                    scope->test_expression);
    (void)copy_text(item->install_component,
                    sizeof(item->install_component),
                    scope->install_component);
}

/* Add a direct path to a scope, or merge it with that scope's existing item. */
static UmiStatus add_direct_item(UmiBuildAutomation *automation,
                                 const UmiBuildAutomationScope *scope,
                                 const UmiBuildAutomationChange *change,
                                 UmiBuildAutomationActionFlags actions)
{
    size_t item_index = find_item_index(automation, scope->scope_id);
    UmiBuildAutomationPlanItem *item;

    if (item_index == automation->item_count) {
        if (automation->item_count >= UMI_BUILD_AUTOMATION_MAX_SCOPES) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        item_index = automation->item_count++;
        initialise_item(&automation->items[item_index], scope);
    }
    item = &automation->items[item_index];
    item->actions |= actions;
    item->changed_file_count += 1U;
    item->direct_change = 1;
    if (item->changed_file_count == 1U) {
        (void)snprintf(item->reason,
                       sizeof(item->reason),
                       "%s changed: %s",
                       umi_build_automation_change_kind_text(change->kind),
                       change->path);
    } else {
        (void)snprintf(item->reason,
                       sizeof(item->reason),
                       "%zu changed files affect this scope",
                       item->changed_file_count);
    }
    return UMI_STATUS_OK;
}

/* Select a consumer because a shared or workspace-wide scope changed. */
static UmiStatus add_inherited_item(UmiBuildAutomation *automation,
                                    const UmiBuildAutomationScope *scope,
                                    const char *provider_name)
{
    size_t item_index = find_item_index(automation, scope->scope_id);
    UmiBuildAutomationPlanItem *item;

    if (item_index == automation->item_count) {
        if (automation->item_count >= UMI_BUILD_AUTOMATION_MAX_SCOPES) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        item_index = automation->item_count++;
        initialise_item(&automation->items[item_index], scope);
    }
    item = &automation->items[item_index];
    item->actions |= scope->default_actions &
        (UMI_BUILD_AUTOMATION_ACTION_BUILD |
         UMI_BUILD_AUTOMATION_ACTION_TEST |
         UMI_BUILD_AUTOMATION_ACTION_STAGE);
    item->inherited_change = 1;
    if (!item->direct_change) {
        (void)snprintf(item->reason,
                       sizeof(item->reason),
                       "Consumes changed shared scope: %s",
                       provider_name != NULL ? provider_name : "workspace");
    }
    return UMI_STATUS_OK;
}

/* Sort direct work first, then use stable scope identifiers for repeatability. */
static int compare_plan_items(const void *left_pointer,
                              const void *right_pointer)
{
    const UmiBuildAutomationPlanItem *left =
        (const UmiBuildAutomationPlanItem *)left_pointer;
    const UmiBuildAutomationPlanItem *right =
        (const UmiBuildAutomationPlanItem *)right_pointer;

    if (left->direct_change != right->direct_change) {
        return left->direct_change ? -1 : 1;
    }
    return strcmp(left->scope_id, right->scope_id);
}

/* Prepare one caller-owned scope with safe defaults and normalised paths. */
void umi_build_automation_scope_init(UmiBuildAutomationScope *scope,
                                     const char *scope_id,
                                     const char *display_name,
                                     const char *source_prefix)
{
    if (scope == NULL) {
        return;
    }
    (void)memset(scope, 0, sizeof(*scope));
    scope->structure_size = (uint32_t)sizeof(*scope);
    scope->api_version = UMI_BUILD_AUTOMATION_API_VERSION;
    (void)copy_text(scope->scope_id, sizeof(scope->scope_id), scope_id);
    (void)copy_text(scope->display_name,
                    sizeof(scope->display_name),
                    display_name);
    (void)normalise_path(source_prefix != NULL ? source_prefix : "",
                         scope->source_prefix,
                         sizeof(scope->source_prefix));
    scope->default_actions = UMI_BUILD_AUTOMATION_ACTION_BUILD |
                             UMI_BUILD_AUTOMATION_ACTION_TEST;
    scope->enabled = 1;
}

/* Prepare changed-path evidence and infer its kind from the file name. */
void umi_build_automation_change_init(UmiBuildAutomationChange *change,
                                      const char *path,
                                      int deleted)
{
    if (change == NULL) {
        return;
    }
    (void)memset(change, 0, sizeof(*change));
    change->structure_size = (uint32_t)sizeof(*change);
    change->api_version = UMI_BUILD_AUTOMATION_API_VERSION;
    (void)normalise_path(path != NULL ? path : "",
                         change->path,
                         sizeof(change->path));
    change->kind = umi_build_automation_classify_path(change->path);
    change->deleted = deleted != 0;
}

/* Classify common project files without opening or modifying their content. */
UmiBuildAutomationChangeKind umi_build_automation_classify_path(
    const char *path)
{
    char normalised[UMI_BUILD_PATH_CAPACITY];
    const char *name;

    if (normalise_path(path != NULL ? path : "",
                       normalised,
                       sizeof(normalised)) != UMI_STATUS_OK ||
        normalised[0] == '\0') {
        return UMI_BUILD_AUTOMATION_CHANGE_UNKNOWN;
    }
    name = file_name_from_path(normalised);

    if (text_equals_case_insensitive(name, "CMakeLists.txt") ||
        text_equals_case_insensitive(name, "CMakePresets.json") ||
        text_equals_case_insensitive(name, "CMakeUserPresets.json") ||
        text_equals_case_insensitive(name, "Makefile") ||
        path_has_extension(name, ".cmake") ||
        path_has_extension(name, ".ninja") ||
        path_has_extension(name, ".mk")) {
        return UMI_BUILD_AUTOMATION_CHANGE_BUILD_DEFINITION;
    }
    if (text_equals_case_insensitive(name, "application.umicom.yaml") ||
        text_equals_case_insensitive(name, "application.umicom.yml") ||
        path_has_extension(name, ".manifest")) {
        return UMI_BUILD_AUTOMATION_CHANGE_MANIFEST;
    }
    if (path_has_segment(normalised, "tests") ||
        path_has_segment(normalised, "test")) {
        return UMI_BUILD_AUTOMATION_CHANGE_TEST;
    }
    if (path_has_extension(name, ".c") ||
        path_has_extension(name, ".cc") ||
        path_has_extension(name, ".cpp") ||
        path_has_extension(name, ".cxx")) {
        return UMI_BUILD_AUTOMATION_CHANGE_SOURCE;
    }
    if (path_has_extension(name, ".h") ||
        path_has_extension(name, ".hh") ||
        path_has_extension(name, ".hpp") ||
        path_has_extension(name, ".hxx")) {
        return path_has_segment(normalised, "include")
            ? UMI_BUILD_AUTOMATION_CHANGE_PUBLIC_HEADER
            : UMI_BUILD_AUTOMATION_CHANGE_PRIVATE_HEADER;
    }
    if (path_has_extension(name, ".s") ||
        path_has_extension(name, ".asm") ||
        path_has_extension(name, ".S")) {
        return UMI_BUILD_AUTOMATION_CHANGE_ASSEMBLY;
    }
    if (path_has_extension(name, ".in") ||
        path_has_extension(name, ".inc")) {
        return UMI_BUILD_AUTOMATION_CHANGE_GENERATED_INPUT;
    }
    if (path_has_segment(normalised, "docs") ||
        strncmp(name, "README", 6U) == 0 ||
        path_has_extension(name, ".md") ||
        path_has_extension(name, ".rst") ||
        path_has_extension(name, ".adoc")) {
        return UMI_BUILD_AUTOMATION_CHANGE_DOCUMENTATION;
    }
    if (path_has_extension(name, ".svg") ||
        path_has_extension(name, ".png") ||
        path_has_extension(name, ".jpg") ||
        path_has_extension(name, ".jpeg") ||
        path_has_extension(name, ".ico") ||
        path_has_extension(name, ".css") ||
        path_has_extension(name, ".ui") ||
        path_has_extension(name, ".gresource.xml")) {
        return UMI_BUILD_AUTOMATION_CHANGE_RESOURCE;
    }
    return UMI_BUILD_AUTOMATION_CHANGE_UNKNOWN;
}

/* Allocate the large bounded planning collections on the heap. */
UmiStatus umi_build_automation_create(UmiBuildAutomation **out_automation)
{
    UmiBuildAutomation *automation;

    if (out_automation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_automation = NULL;
    automation = (UmiBuildAutomation *)calloc(1U, sizeof(*automation));
    if (automation == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    automation->revision = 1U;
    automation->snapshot.structure_size =
        (uint32_t)sizeof(automation->snapshot);
    automation->snapshot.api_version = UMI_BUILD_AUTOMATION_API_VERSION;
    automation->snapshot.revision = automation->revision;
    *out_automation = automation;
    return UMI_STATUS_OK;
}

/* Release the one allocation owned by the opaque coordinator. */
void umi_build_automation_destroy(UmiBuildAutomation *automation)
{
    free(automation);
}

/* Clear all evidence and advance the revision for observing clients. */
void umi_build_automation_clear(UmiBuildAutomation *automation)
{
    if (automation == NULL) {
        return;
    }
    (void)memset(automation->scopes, 0, sizeof(automation->scopes));
    (void)memset(automation->changes, 0, sizeof(automation->changes));
    (void)memset(automation->items, 0, sizeof(automation->items));
    automation->scope_count = 0U;
    automation->change_count = 0U;
    automation->item_count = 0U;
    automation->revision += 1U;
    (void)memset(&automation->snapshot, 0, sizeof(automation->snapshot));
    automation->snapshot.structure_size =
        (uint32_t)sizeof(automation->snapshot);
    automation->snapshot.api_version = UMI_BUILD_AUTOMATION_API_VERSION;
    automation->snapshot.revision = automation->revision;
}

/* Finish one batch without forcing clients to rediscover stable scope data. */
void umi_build_automation_clear_changes(UmiBuildAutomation *automation)
{
    if (automation == NULL) {
        return;
    }
    (void)memset(automation->changes, 0, sizeof(automation->changes));
    (void)memset(automation->items, 0, sizeof(automation->items));
    automation->change_count = 0U;
    automation->item_count = 0U;
    automation->revision += 1U;
    (void)memset(&automation->snapshot, 0, sizeof(automation->snapshot));
    automation->snapshot.structure_size =
        (uint32_t)sizeof(automation->snapshot);
    automation->snapshot.api_version = UMI_BUILD_AUTOMATION_API_VERSION;
    automation->snapshot.scope_count = automation->scope_count;
    automation->snapshot.revision = automation->revision;
}

/* Validate, normalise and upsert a copied product-scope definition. */
UmiStatus umi_build_automation_register_scope(
    UmiBuildAutomation *automation,
    const UmiBuildAutomationScope *scope)
{
    UmiBuildAutomationScope copy;
    size_t index;
    UmiStatus status;

    if (automation == NULL || scope == NULL ||
        scope->structure_size < sizeof(*scope) ||
        scope->api_version != UMI_BUILD_AUTOMATION_API_VERSION ||
        scope->scope_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    copy = *scope;
    status = normalise_path(scope->source_prefix,
                            copy.source_prefix,
                            sizeof(copy.source_prefix));
    if (status != UMI_STATUS_OK) {
        return status;
    }
    index = find_scope_index(automation, copy.scope_id);
    if (index == automation->scope_count) {
        if (automation->scope_count >= UMI_BUILD_AUTOMATION_MAX_SCOPES) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        automation->scope_count += 1U;
    }
    automation->scopes[index] = copy;
    automation->item_count = 0U;
    automation->snapshot.evaluated = 0;
    automation->revision += 1U;
    return UMI_STATUS_OK;
}

/* Validate, normalise and upsert one copied changed-path record. */
UmiStatus umi_build_automation_add_change(
    UmiBuildAutomation *automation,
    const UmiBuildAutomationChange *change)
{
    UmiBuildAutomationChange copy;
    size_t index;
    UmiStatus status;

    if (automation == NULL || change == NULL ||
        change->structure_size < sizeof(*change) ||
        change->api_version != UMI_BUILD_AUTOMATION_API_VERSION ||
        change->path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    copy = *change;
    status = normalise_path(change->path,
                            copy.path,
                            sizeof(copy.path));
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (copy.kind == UMI_BUILD_AUTOMATION_CHANGE_UNKNOWN) {
        copy.kind = umi_build_automation_classify_path(copy.path);
    }
    for (index = 0U; index < automation->change_count; ++index) {
        if (strcmp(automation->changes[index].path, copy.path) == 0) {
            automation->changes[index] = copy;
            automation->item_count = 0U;
            automation->snapshot.evaluated = 0;
            automation->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    if (automation->change_count >= UMI_BUILD_AUTOMATION_MAX_CHANGES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    automation->changes[automation->change_count++] = copy;
    automation->item_count = 0U;
    automation->snapshot.evaluated = 0;
    automation->revision += 1U;
    return UMI_STATUS_OK;
}

/* Evaluate direct ownership first, then add safe transitive consumers. */
UmiStatus umi_build_automation_evaluate(UmiBuildAutomation *automation)
{
    size_t change_index;
    size_t scope_index;
    size_t item_index;
    UmiStatus status;

    if (automation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    automation->item_count = 0U;
    (void)memset(automation->items, 0, sizeof(automation->items));

    for (change_index = 0U;
         change_index < automation->change_count;
         ++change_index) {
        const UmiBuildAutomationChange *change =
            &automation->changes[change_index];
        const UmiBuildAutomationScope *scope =
            find_scope_for_path(automation, change->path);
        UmiBuildAutomationActionFlags actions;

        if (scope == NULL) {
            return UMI_STATUS_NOT_FOUND;
        }
        actions = actions_for_change(change->kind, scope->default_actions);
        status = add_direct_item(automation, scope, change, actions);
        if (status != UMI_STATUS_OK) {
            return status;
        }
        if ((actions & (UMI_BUILD_AUTOMATION_ACTION_CONFIGURE |
                        UMI_BUILD_AUTOMATION_ACTION_BUILD |
                        UMI_BUILD_AUTOMATION_ACTION_TEST |
                        UMI_BUILD_AUTOMATION_ACTION_STAGE)) == 0U) {
            continue;
        }
        if ((scope->flags & UMI_BUILD_AUTOMATION_SCOPE_WORKSPACE_WIDE) != 0U) {
            for (scope_index = 0U;
                 scope_index < automation->scope_count;
                 ++scope_index) {
                const UmiBuildAutomationScope *candidate =
                    &automation->scopes[scope_index];

                if (!candidate->enabled || candidate == scope) {
                    continue;
                }
                status = add_inherited_item(
                    automation, candidate, scope->display_name);
                if (status != UMI_STATUS_OK) {
                    return status;
                }
            }
        } else if ((scope->flags &
                    UMI_BUILD_AUTOMATION_SCOPE_SHARED_PROVIDER) != 0U) {
            for (scope_index = 0U;
                 scope_index < automation->scope_count;
                 ++scope_index) {
                const UmiBuildAutomationScope *candidate =
                    &automation->scopes[scope_index];

                if (!candidate->enabled || candidate == scope ||
                    (candidate->flags &
                     UMI_BUILD_AUTOMATION_SCOPE_CONSUMES_SHARED) == 0U) {
                    continue;
                }
                status = add_inherited_item(
                    automation, candidate, scope->display_name);
                if (status != UMI_STATUS_OK) {
                    return status;
                }
            }
        }
    }

    qsort(automation->items,
          automation->item_count,
          sizeof(automation->items[0]),
          compare_plan_items);
    (void)memset(&automation->snapshot, 0, sizeof(automation->snapshot));
    automation->snapshot.structure_size =
        (uint32_t)sizeof(automation->snapshot);
    automation->snapshot.api_version = UMI_BUILD_AUTOMATION_API_VERSION;
    automation->snapshot.scope_count = automation->scope_count;
    automation->snapshot.change_count = automation->change_count;
    automation->snapshot.selected_count = automation->item_count;
    for (item_index = 0U; item_index < automation->item_count; ++item_index) {
        UmiBuildAutomationPlanItem *item = &automation->items[item_index];

        item->order = item_index;
        automation->snapshot.actions |= item->actions;
        if (item->direct_change) {
            automation->snapshot.direct_count += 1U;
        }
        if (item->inherited_change) {
            automation->snapshot.inherited_count += 1U;
        }
    }
    automation->revision += 1U;
    automation->snapshot.revision = automation->revision;
    automation->snapshot.evaluated = 1;
    return UMI_STATUS_OK;
}

/* Copy one evaluated plan item without exposing internal array storage. */
UmiStatus umi_build_automation_item_at(
    const UmiBuildAutomation *automation,
    size_t position,
    UmiBuildAutomationPlanItem *out_item)
{
    if (automation == NULL || out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!automation->snapshot.evaluated) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (position >= automation->item_count) {
        return UMI_STATUS_NOT_FOUND;
    }
    *out_item = automation->items[position];
    return UMI_STATUS_OK;
}

/* Return zero until evaluation prevents callers from reading stale plans. */
size_t umi_build_automation_count(const UmiBuildAutomation *automation)
{
    return automation != NULL && automation->snapshot.evaluated
        ? automation->item_count
        : 0U;
}

/* Copy aggregate state for a command, dashboard or test surface. */
UmiStatus umi_build_automation_snapshot(
    const UmiBuildAutomation *automation,
    UmiBuildAutomationSnapshot *out_snapshot)
{
    if (automation == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_snapshot = automation->snapshot;
    return UMI_STATUS_OK;
}

/* Append one label to a bounded action list without writing past the buffer. */
static void append_action_text(char *out_text,
                               size_t capacity,
                               const char *label,
                               int *first)
{
    const size_t used = strlen(out_text);

    if (used >= capacity) {
        return;
    }
    (void)snprintf(out_text + used,
                   capacity - used,
                   "%s%s",
                   *first ? "" : ",",
                   label);
    *first = 0;
}

/* Format action flags in lifecycle order so reports remain deterministic. */
const char *umi_build_automation_action_text(
    UmiBuildAutomationActionFlags actions,
    char *out_text,
    size_t capacity)
{
    int first = 1;

    if (out_text == NULL || capacity == 0U) {
        return NULL;
    }
    out_text[0] = '\0';
    if ((actions & UMI_BUILD_AUTOMATION_ACTION_CONFIGURE) != 0U) {
        append_action_text(out_text, capacity, "configure", &first);
    }
    if ((actions & UMI_BUILD_AUTOMATION_ACTION_BUILD) != 0U) {
        append_action_text(out_text, capacity, "build", &first);
    }
    if ((actions & UMI_BUILD_AUTOMATION_ACTION_TEST) != 0U) {
        append_action_text(out_text, capacity, "test", &first);
    }
    if ((actions & UMI_BUILD_AUTOMATION_ACTION_STAGE) != 0U) {
        append_action_text(out_text, capacity, "stage", &first);
    }
    if ((actions & UMI_BUILD_AUTOMATION_ACTION_DEPLOY) != 0U) {
        append_action_text(out_text, capacity, "deploy", &first);
    }
    if ((actions & UMI_BUILD_AUTOMATION_ACTION_DOCUMENT) != 0U) {
        append_action_text(out_text, capacity, "document", &first);
    }
    if (first) {
        (void)snprintf(out_text, capacity, "none");
    }
    return out_text;
}

/* Translate change kinds into short explanations suitable for plan output. */
const char *umi_build_automation_change_kind_text(
    UmiBuildAutomationChangeKind kind)
{
    switch (kind) {
        case UMI_BUILD_AUTOMATION_CHANGE_SOURCE: return "source";
        case UMI_BUILD_AUTOMATION_CHANGE_PUBLIC_HEADER: return "public header";
        case UMI_BUILD_AUTOMATION_CHANGE_PRIVATE_HEADER: return "private header";
        case UMI_BUILD_AUTOMATION_CHANGE_ASSEMBLY: return "assembly source";
        case UMI_BUILD_AUTOMATION_CHANGE_BUILD_DEFINITION: return "build definition";
        case UMI_BUILD_AUTOMATION_CHANGE_TEST: return "test source";
        case UMI_BUILD_AUTOMATION_CHANGE_RESOURCE: return "resource";
        case UMI_BUILD_AUTOMATION_CHANGE_DOCUMENTATION: return "documentation";
        case UMI_BUILD_AUTOMATION_CHANGE_MANIFEST: return "application manifest";
        case UMI_BUILD_AUTOMATION_CHANGE_GENERATED_INPUT: return "generated input";
        case UMI_BUILD_AUTOMATION_CHANGE_UNKNOWN:
        default: return "unclassified input";
    }
}
