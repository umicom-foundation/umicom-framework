/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_automation.c
 *
 * PURPOSE:
 *   Connect source-control change discovery, application manifests, the
 *   Framework automated planner and the existing native build executor.
 *
 * ARCHITECTURE:
 *   The command is a thin adapter. Git remains behind the Framework VCS
 *   provider, planning remains in umicom_build and process execution remains
 *   in the toolchain build service. No shell command is assembled here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "cli.h"

#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* State shared with the bounded manifest-directory visitor. */
typedef struct UmiCliAutomationApplicationScan {
    UmiBuildAutomation *automation;
    char applications_root[UMI_PATH_CAPACITY];
} UmiCliAutomationApplicationScan;

/* Carry the continuous service and safe callback state into watcher events. */
typedef struct UmiCliAutomationWatch {
    UmiBuildContinuousIntegration *continuous;
    UmiClock clock;
    char source_root[UMI_PATH_CAPACITY];
    UmiStatus event_status;
    size_t ignored_event_count;
    int priming;
} UmiCliAutomationWatch;

/* Keep the durable broker beside the update callback that publishes to it. */
typedef struct UmiCliAutomationPublication {
    UmiChangeBroker *broker;
} UmiCliAutomationPublication;

/* Let repository discovery feed either a one-shot or continuous planner. */
typedef UmiStatus (*UmiCliAutomationChangeSink)(void *user_data,
                                                const char *path,
                                                int deleted);

/* A signal handler may safely change only this small atomic flag. */
static volatile sig_atomic_t automation_stop_requested = 0;

/* Return the value following a named command option, or NULL when absent. */
static const char *automation_option_value(int argc,
                                           char **argv,
                                           const char *option)
{
    int index;

    for (index = 0; index + 1 < argc; ++index) {
        if (strcmp(argv[index], option) == 0) {
            return argv[index + 1];
        }
    }
    return NULL;
}

/* Report whether a switch appears anywhere in this command's arguments. */
static int automation_has_flag(int argc, char **argv, const char *flag)
{
    int index;

    for (index = 0; index < argc; ++index) {
        if (strcmp(argv[index], flag) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Parse the optional parallel-job count without the overflow ambiguity of
 * atoi. Zero means that the underlying build tool may choose its default. */
static UmiStatus automation_parse_jobs(const char *text, int *out_jobs)
{
    char *end = NULL;
    long value;

    if (out_jobs == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_jobs = 0;
    if (text == NULL) {
        return UMI_STATUS_OK;
    }
    errno = 0;
    value = strtol(text, &end, 10);
    if (errno == ERANGE || end == text || end == NULL || *end != '\0' ||
        value < 0L || value > INT_MAX) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_jobs = (int)value;
    return UMI_STATUS_OK;
}

/* Parse a positive millisecond option without narrowing an overflowing value. */
static UmiStatus automation_parse_milliseconds(const char *text,
                                               uint32_t default_value,
                                               uint32_t *out_value)
{
    char *end = NULL;
    unsigned long value;

    if (out_value == NULL || default_value == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_value = default_value;
    if (text == NULL) {
        return UMI_STATUS_OK;
    }
    errno = 0;
    value = strtoul(text, &end, 10);
    if (errno == ERANGE || end == text || end == NULL || *end != '\0' ||
        value == 0UL || value > UINT32_MAX) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_value = (uint32_t)value;
    return UMI_STATUS_OK;
}

/* Copy a command-owned string only when the destination can hold it. */
static UmiStatus automation_copy_text(char *destination,
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

/* Convert path separators so paths can be compared across supported hosts. */
static void automation_normalise_separators(char *path)
{
    size_t index;

    if (path == NULL) {
        return;
    }
    for (index = 0U; path[index] != '\0'; ++index) {
        if (path[index] == '\\') {
            path[index] = '/';
        }
    }
}

/* Resolve the workspace once so watcher events and manifests share one base. */
static UmiStatus automation_resolve_project_root(UmiCliContext *context)
{
    char current_directory[UMI_PATH_CAPACITY];
    char absolute_root[UMI_PATH_CAPACITY];
    UmiStatus status;

    if (context == NULL || context->project_root[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_fs_current_directory(current_directory,
                                      sizeof(current_directory));
    if (status == UMI_STATUS_OK) {
        status = umi_path_absolute(context->project_root,
                                   current_directory,
                                   absolute_root,
                                   sizeof(absolute_root));
    }
    if (status == UMI_STATUS_OK) {
        status = automation_copy_text(context->project_root,
                                      sizeof(context->project_root),
                                      absolute_root);
    }
    return status;
}

/*
 * Build a CTest expression from a directory slug. Separators are accepted in
 * dotted, underscored or dashed test names so existing modules remain usable.
 */
static UmiStatus automation_test_expression(const char *slug,
                                            char *out_expression,
                                            size_t capacity)
{
    static const char SUFFIX[] = "[.]|^applications[.]";
    const size_t suffix_length = sizeof(SUFFIX) - 1U;
    size_t input_index;
    size_t output_index = 0U;

    if (slug == NULL || out_expression == NULL ||
        capacity <= suffix_length + 1U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    out_expression[output_index++] = '^';
    for (input_index = 0U; slug[input_index] != '\0'; ++input_index) {
        if (slug[input_index] == '-') {
            const char separator_expression[] = "[._-]";
            const size_t separator_length = sizeof(separator_expression) - 1U;

            /* Reserve room for the separator, integration suffix and NUL. */
            if (output_index + separator_length + suffix_length + 1U >
                capacity) {
                out_expression[0] = '\0';
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            (void)memcpy(out_expression + output_index,
                         separator_expression,
                         separator_length);
            output_index += separator_length;
        } else {
            /* Reserve room for this character, integration suffix and NUL. */
            if (output_index + suffix_length + 2U > capacity) {
                out_expression[0] = '\0';
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_expression[output_index++] = slug[input_index];
        }
    }
    (void)memcpy(out_expression + output_index, SUFFIX, suffix_length);
    output_index += suffix_length;
    out_expression[output_index] = '\0';
    return UMI_STATUS_OK;
}

/* Register the workspace and shared Framework scopes before applications. */
static UmiStatus automation_register_foundation_scopes(
    UmiBuildAutomation *automation)
{
    UmiBuildAutomationScope scope;
    UmiStatus status;

    umi_build_automation_scope_init(
        &scope, "workspace", "Umicom workspace", "");
    status = automation_copy_text(scope.build_target,
                                  sizeof(scope.build_target),
                                  "umicom-products");
    if (status == UMI_STATUS_OK) {
        status = automation_copy_text(scope.test_build_target,
                                      sizeof(scope.test_build_target),
                                      "umicom-tests");
    }
    scope.flags = UMI_BUILD_AUTOMATION_SCOPE_WORKSPACE_WIDE;
    if (status == UMI_STATUS_OK) {
        status = umi_build_automation_register_scope(automation, &scope);
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }

    umi_build_automation_scope_init(
        &scope, "framework", "Umicom Framework", "framework");
    status = automation_copy_text(scope.version,
                                  sizeof(scope.version),
                                  UMICOM_FRAMEWORK_VERSION_STRING);
    if (status == UMI_STATUS_OK) {
        status = automation_copy_text(scope.build_target,
                                  sizeof(scope.build_target),
                                  "umicom-products");
    }
    if (status == UMI_STATUS_OK) {
        status = automation_copy_text(scope.test_build_target,
                                      sizeof(scope.test_build_target),
                                      "umicom-tests");
    }
    if (status == UMI_STATUS_OK) {
        status = automation_copy_text(scope.install_component,
                                      sizeof(scope.install_component),
                                      "Framework");
    }
    scope.flags = UMI_BUILD_AUTOMATION_SCOPE_SHARED_PROVIDER;
    return status == UMI_STATUS_OK
        ? umi_build_automation_register_scope(automation, &scope)
        : status;
}

/*
 * Register one application directory discovered through its checked-in
 * manifest. The generated aggregate target exists independently of whether a
 * graphical or headless frontend is active in the selected CMake preset.
 */
static UmiStatus automation_manifest_visitor(const UmiFileInfo *info,
                                             void *user_data)
{
    UmiCliAutomationApplicationScan *scan =
        (UmiCliAutomationApplicationScan *)user_data;
    UmiApplicationManifest manifest;
    UmiBuildAutomationScope scope;
    char path[UMI_PATH_CAPACITY];
    char slug[UMI_BUILD_ID_CAPACITY];
    char prefix[UMI_BUILD_PATH_CAPACITY];
    char target[UMI_BUILD_NAME_CAPACITY];
    char test_target[UMI_BUILD_NAME_CAPACITY];
    char expression[UMI_BUILD_ARGUMENT_CAPACITY];
    const char *relative;
    const char *separator;
    size_t root_length;
    size_t slug_length;
    UmiStatus status;

    if (info == NULL || scan == NULL ||
        info->kind != UMI_FILE_KIND_REGULAR ||
        strcmp(info->name, "application.umicom.yaml") != 0) {
        return UMI_STATUS_OK;
    }
    status = automation_copy_text(path, sizeof(path), info->path);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    automation_normalise_separators(path);
    root_length = strlen(scan->applications_root);
    if (strncmp(path, scan->applications_root, root_length) != 0 ||
        path[root_length] != '/') {
        return UMI_STATUS_INVALID_STATE;
    }
    relative = path + root_length + 1U;
    separator = strchr(relative, '/');
    if (separator == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }
    slug_length = (size_t)(separator - relative);
    if (slug_length == 0U || slug_length >= sizeof(slug)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(slug, relative, slug_length);
    slug[slug_length] = '\0';

    status = umi_application_manifest_load(info->path, &manifest);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    {
        const int prefix_length = snprintf(prefix,
                                           sizeof(prefix),
                                           "applications/%s",
                                           slug);
        const int target_length = snprintf(target,
                                           sizeof(target),
                                           "umicom-product-%s",
                                           slug);
        const int test_target_length = snprintf(test_target,
                                                sizeof(test_target),
                                                "umicom-tests-%s",
                                                slug);

        /* Reject truncation so two long names can never become one target. */
        if (prefix_length < 0 || (size_t)prefix_length >= sizeof(prefix) ||
            target_length < 0 || (size_t)target_length >= sizeof(target) ||
            test_target_length < 0 ||
            (size_t)test_target_length >= sizeof(test_target)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }
    status = automation_test_expression(slug,
                                        expression,
                                        sizeof(expression));
    if (status != UMI_STATUS_OK) {
        return status;
    }
    umi_build_automation_scope_init(
        &scope, slug, manifest.name, prefix);
    status = automation_copy_text(scope.version,
                                  sizeof(scope.version),
                                  manifest.version);
    if (status == UMI_STATUS_OK) {
        status = automation_copy_text(scope.build_target,
                                  sizeof(scope.build_target),
                                  target);
    }
    if (status == UMI_STATUS_OK) {
        status = automation_copy_text(scope.test_build_target,
                                      sizeof(scope.test_build_target),
                                      test_target);
    }
    if (status == UMI_STATUS_OK) {
        status = automation_copy_text(scope.test_expression,
                                      sizeof(scope.test_expression),
                                      expression);
    }
    scope.flags = UMI_BUILD_AUTOMATION_SCOPE_CONSUMES_SHARED;
    return status == UMI_STATUS_OK
        ? umi_build_automation_register_scope(scan->automation, &scope)
        : status;
}

/* Discover every immediate application manifest without hard-coding products. */
static UmiStatus automation_register_application_scopes(
    UmiBuildAutomation *automation,
    const char *project_root)
{
    UmiCliAutomationApplicationScan scan;
    UmiDirectoryWalkOptions options = umi_directory_walk_options_default();
    UmiStatus status;

    (void)memset(&scan, 0, sizeof(scan));
    scan.automation = automation;
    status = umi_fs_join(scan.applications_root,
                         sizeof(scan.applications_root),
                         project_root,
                         "applications");
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Collapse relative segments such as "." before comparing walked paths.
     * This keeps an explicit relative workspace path equivalent to the default
     * absolute working-directory path. */
    status = umi_fs_normalise(scan.applications_root);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    automation_normalise_separators(scan.applications_root);
    options.recursive = 1;
    options.max_depth = 1U;
    options.include_files = 1;
    options.include_directories = 0;
    options.include_hidden = 0;
    options.follow_symbolic_links = 0;
    return umi_directory_walk(scan.applications_root,
                              &options,
                              automation_manifest_visitor,
                              &scan);
}

/* Read working-tree and index changes once, then pass each path to a caller. */
static UmiStatus automation_visit_repository_changes(
    const char *project_root,
    UmiCliAutomationChangeSink sink,
    void *sink_user_data)
{
    UmiVcsProvider provider;
    UmiVcsRepository *repository = NULL;
    UmiVcsChangeList *changes = NULL;
    UmiVcsBranch branch;
    size_t index;
    UmiStatus status;

    /* Both values are required because the visitor calls the sink directly
     * for every discovered path. */
    if (project_root == NULL || project_root[0] == '\0' || sink == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&provider, 0, sizeof(provider));
    (void)memset(&branch, 0, sizeof(branch));
    status = umi_vcs_git_cli_provider(&provider);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = umi_vcs_repository_create(project_root, &provider, &repository);
    if (status != UMI_STATUS_OK) {
        if (provider.destroy != NULL) {
            provider.destroy(provider.instance);
        }
        return status;
    }
    status = umi_vcs_change_list_create(&changes);
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_repository_status(repository, changes, &branch);
    }
    if (status == UMI_STATUS_OK) {
        for (index = 0U;
             index < umi_vcs_change_list_count(changes);
             ++index) {
            const UmiVcsChange *vcs_change =
                umi_vcs_change_list_at(changes, index);
            const int deleted = vcs_change != NULL &&
                (vcs_change->index_state == UMI_VCS_CHANGE_DELETED ||
                 vcs_change->worktree_state == UMI_VCS_CHANGE_DELETED);

            if (vcs_change == NULL) {
                status = UMI_STATUS_INVALID_STATE;
                break;
            }
            status = sink(sink_user_data, vcs_change->path, deleted);
            if (status != UMI_STATUS_OK) {
                break;
            }
        }
    }
    umi_vcs_change_list_destroy(changes);
    umi_vcs_repository_destroy(repository);
    return status;
}

/* Adapt repository discovery to the one-shot affected-scope planner. */
static UmiStatus automation_planner_change_sink(void *user_data,
                                                const char *path,
                                                int deleted)
{
    UmiBuildAutomationChange change;

    if (user_data == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_build_automation_change_init(&change, path, deleted);
    return umi_build_automation_add_change(
        (UmiBuildAutomation *)user_data, &change);
}

/* Add working-tree and index changes exposed by the Framework VCS provider. */
static UmiStatus automation_discover_changes(UmiBuildAutomation *automation,
                                             const char *project_root)
{
    return automation_visit_repository_changes(
        project_root, automation_planner_change_sink, automation);
}

/* Convert a Framework clock reading to milliseconds without losing ordering. */
static uint64_t automation_clock_milliseconds(const UmiClock *clock,
                                              int wall_time)
{
    UmiClockNowFn now;

    if (clock == NULL) {
        return 0U;
    }
    now = wall_time ? clock->wall_nanoseconds : clock->monotonic_nanoseconds;
    return now != NULL ? now(clock) / 1000000U : 0U;
}

/* Stop the watch loop at a safe boundary after the user presses Ctrl+C. */
static void automation_stop_handler(int signal_number)
{
    (void)signal_number;
    automation_stop_requested = 1;
}

/* Match one complete top-level path segment, with an optional name prefix. */
static int automation_matches_root_segment(const char *path,
                                           const char *name,
                                           int prefix_match)
{
    const char *separator;
    size_t segment_length;
    size_t name_length;

    if (path == NULL || name == NULL) {
        return 0;
    }
    separator = strchr(path, '/');
    segment_length = separator != NULL
        ? (size_t)(separator - path)
        : strlen(path);
    name_length = strlen(name);
    return prefix_match
        ? segment_length >= name_length &&
              strncmp(path, name, name_length) == 0
        : segment_length == name_length &&
              strncmp(path, name, name_length) == 0;
}

/* Ignore generated output so a successful build cannot trigger itself again. */
static int automation_watch_ignores_path(const char *relative_path)
{
    static const char *const GENERATED_DIRECTORIES[] = {
        ".git", ".umicom", "out", "dist", "install", "_CPack_Packages"
    };
    static const char *const GENERATED_SUFFIXES[] = {
        ".a", ".dll", ".dylib", ".exe", ".exp", ".ilk", ".lib",
        ".o", ".obj", ".pdb", ".so", ".tmp", ".temp", ".log"
    };
    size_t index;
    size_t path_length;

    if (relative_path == NULL || relative_path[0] == '\0' ||
        strncmp(relative_path, "../", 3U) == 0) {
        return 1;
    }
    for (index = 0U;
         index < sizeof(GENERATED_DIRECTORIES) /
                     sizeof(GENERATED_DIRECTORIES[0]);
         ++index) {
        if (automation_matches_root_segment(
                relative_path, GENERATED_DIRECTORIES[index], 0)) {
            return 1;
        }
    }
    if (automation_matches_root_segment(relative_path, "build", 0) ||
        automation_matches_root_segment(relative_path, "build-", 1) ||
        automation_matches_root_segment(relative_path, "build_", 1) ||
        automation_matches_root_segment(relative_path, "cmake-build-", 1)) {
        return 1;
    }
    path_length = strlen(relative_path);
    for (index = 0U;
         index < sizeof(GENERATED_SUFFIXES) / sizeof(GENERATED_SUFFIXES[0]);
         ++index) {
        const size_t suffix_length = strlen(GENERATED_SUFFIXES[index]);

        if (path_length >= suffix_length &&
            strcmp(relative_path + path_length - suffix_length,
                   GENERATED_SUFFIXES[index]) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Feed a changed repository path into the continuous debounce lifecycle. */
static UmiStatus automation_continuous_change_sink(void *user_data,
                                                   const char *path,
                                                   int deleted)
{
    UmiCliAutomationWatch *watch = (UmiCliAutomationWatch *)user_data;

    if (watch == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (automation_watch_ignores_path(path)) {
        watch->ignored_event_count += 1U;
        return UMI_STATUS_OK;
    }
    return umi_build_continuous_record_change(
        watch->continuous,
        path,
        deleted,
        automation_clock_milliseconds(&watch->clock, 0));
}

/* Translate one filesystem event into a normalised workspace-relative change. */
static void automation_watch_event_sink(const UmiWatchEvent *event,
                                        void *user_data)
{
    UmiCliAutomationWatch *watch = (UmiCliAutomationWatch *)user_data;
    char relative_path[UMI_PATH_CAPACITY];
    UmiStatus status;

    if (watch == NULL || event == NULL || watch->priming ||
        watch->event_status != UMI_STATUS_OK || event->directory) {
        return;
    }
    if (event->kind == UMI_WATCH_OVERFLOW ||
        event->kind == UMI_WATCH_RESCAN_REQUIRED) {
        /* An overflow means detail was lost, so a root definition change is
         * the safe signal that asks the planner for a complete rebuild. */
        watch->event_status = umi_build_continuous_record_change(
            watch->continuous,
            "CMakeLists.txt",
            0,
            automation_clock_milliseconds(&watch->clock, 0));
        return;
    }
    status = umi_path_relative(watch->source_root,
                               event->path,
                               relative_path,
                               sizeof(relative_path));
    if (status != UMI_STATUS_OK) {
        watch->event_status = status;
        return;
    }
    automation_normalise_separators(relative_path);
    if (automation_watch_ignores_path(relative_path)) {
        watch->ignored_event_count += 1U;
        return;
    }
    watch->event_status = umi_build_continuous_record_change(
        watch->continuous,
        relative_path,
        event->kind == UMI_WATCH_DELETED,
        automation_clock_milliseconds(&watch->clock, 0));
}

/* Persist one successful generation before reporting it to the operator. */
static UmiStatus automation_publish_update(
    const UmiBuildContinuousUpdate *update,
    void *user_data)
{
    UmiCliAutomationPublication *publication =
        (UmiCliAutomationPublication *)user_data;
    char payload[2048];
    UmiStatus status;

    if (publication == NULL || publication->broker == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_build_continuous_update_format_json(
        update, payload, sizeof(payload));
    if (status == UMI_STATUS_OK) {
        status = umi_change_broker_publish(
            publication->broker,
            UMI_BUILD_CONTINUOUS_UPDATE_TOPIC,
            payload,
            update->generation);
    }
    if (status == UMI_STATUS_OK) {
        (void)printf(
            "Update available: %s %s, generation %llu%s\n",
            update->display_name,
            update->version,
            (unsigned long long)update->generation,
            update->restart_required ? " (restart required)" : "");
    }
    return status;
}

/* Print a complete, human-readable plan before any execution is considered. */
static UmiStatus automation_print_plan(const UmiBuildAutomation *automation)
{
    UmiBuildAutomationSnapshot snapshot;
    size_t index;
    UmiStatus status =
        umi_build_automation_snapshot(automation, &snapshot);

    if (status != UMI_STATUS_OK) {
        return status;
    }
    (void)printf("Automated build plan: %zu changed files, %zu affected scopes\n",
                 snapshot.change_count,
                 snapshot.selected_count);
    for (index = 0U; index < snapshot.selected_count; ++index) {
        UmiBuildAutomationPlanItem item;
        char actions[128];

        status = umi_build_automation_item_at(automation, index, &item);
        if (status != UMI_STATUS_OK) {
            return status;
        }
        (void)printf("  %s [%s]\n    %s\n",
                     item.display_name,
                     umi_build_automation_action_text(
                         item.actions, actions, sizeof(actions)),
                     item.reason);
        if (item.build_target[0] != '\0') {
            (void)printf("    build target: %s\n", item.build_target);
        }
        if ((item.actions & UMI_BUILD_AUTOMATION_ACTION_TEST) != 0U &&
            item.test_expression[0] != '\0') {
            (void)printf("    test selection: %s\n", item.test_expression);
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Invoke the established Framework build service with one prepared toolchain.
 * Reusing the caller's context avoids repeating compiler discovery for every
 * selected target, and no shell expression is ever assembled.
 */
static int automation_run_action(UmiCliContext *context,
                                 UmiBuildAction action,
                                 const char *source_root,
                                 const char *preset,
                                 const char *build_directory,
                                 const char *target,
                                 const char *test_expression,
                                 const char *install_prefix,
                                 const char *install_component,
                                 int jobs)
{
    UmiBuildRequest request;
    UmiBuildReport report;
    char build_root[UMI_PATH_CAPACITY];
    char resolved_build_directory[UMI_PATH_CAPACITY];
    UmiStatus status;

    if (context == NULL || !context->environment_ready ||
        source_root == NULL || source_root[0] == '\0') {
        return 1;
    }

    /* CMake installation always needs the concrete binary directory. Derive
     * the same conventional path for presets that the normal build command
     * uses, while preserving an explicit caller override. */
    if (build_directory != NULL && build_directory[0] != '\0') {
        status = automation_copy_text(resolved_build_directory,
                                      sizeof(resolved_build_directory),
                                      build_directory);
    } else {
        status = umi_fs_join(build_root,
                             sizeof(build_root),
                             source_root,
                             "build");
        if (status == UMI_STATUS_OK) {
            status = umi_fs_join(resolved_build_directory,
                                 sizeof(resolved_build_directory),
                                 build_root,
                                 preset);
        }
    }
    if (status != UMI_STATUS_OK) {
        return 1;
    }

    umi_build_request_init(&request);
    request.source_root = source_root;
    request.build_directory = resolved_build_directory;
    request.preset = build_directory != NULL && build_directory[0] != '\0'
        ? NULL
        : preset;
    request.target = target;
    request.test_expression = test_expression;
    request.install_prefix = install_prefix;
    request.install_component = install_component;
    request.jobs = jobs;
    status = umi_build_execute(&context->discovery.profile,
                               &context->environment,
                               action,
                               &request,
                               &report);

    /* Preserve the child tool's final diagnostics so a failed automated run
     * remains as understandable as a direct Umicom build command. */
    if (report.output_truncated) {
        (void)fputs(
            "[umicom] Earlier child-process output was omitted; "
            "the final diagnostic output follows.\n",
            stderr);
    }
    if (report.last_output[0] != '\0') {
        (void)fputs(report.last_output, stdout);
    }
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "Automated %s action failed: %s\n",
                      umi_build_action_text(action),
                      umi_status_text(status));
        return 1;
    }
    return 0;
}

/* Return true when a target has already been executed in this plan. */
static int automation_target_seen(
    char targets[UMI_BUILD_AUTOMATION_MAX_SCOPES][UMI_BUILD_NAME_CAPACITY],
    size_t target_count,
    const char *target)
{
    size_t index;

    for (index = 0U; index < target_count; ++index) {
        if (strcmp(targets[index], target) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Execute configuration, selected product targets, selected tests and opt-in deployment. */
static int automation_execute(UmiCliContext *context,
                              const UmiBuildAutomation *automation,
                              const char *source_root,
                              const char *preset,
                              const char *build_directory,
                              const char *install_prefix,
                              int jobs,
                              int run_tests,
                              int deploy)
{
    UmiBuildAutomationSnapshot snapshot;
    char stable_source_root[UMI_PATH_CAPACITY];
    char built_targets[UMI_BUILD_AUTOMATION_MAX_SCOPES]
                      [UMI_BUILD_NAME_CAPACITY];
    char test_targets[UMI_BUILD_AUTOMATION_MAX_SCOPES]
                     [UMI_BUILD_NAME_CAPACITY];
    size_t built_count = 0U;
    size_t test_count = 0U;
    size_t index;
    int built_all = 0;
    int tested_all = 0;
    UmiStatus status;

    /* Preserve the path before the first full preparation resets the command
     * context. A continuous host keeps the successful compiler profile and
     * environment, so later generations do not repeat the discovery probe. */
    status = automation_copy_text(stable_source_root,
                                  sizeof(stable_source_root),
                                  source_root);
    if (status == UMI_STATUS_OK &&
        !context->discovery.compile_probe_passed) {
        status = umi_cli_context_prepare(
            context, stable_source_root, 0, 0);
    }
    if (status != UMI_STATUS_OK) {
        return 1;
    }
    status = automation_resolve_project_root(context);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "Unable to resolve workspace path: %s\n",
                      umi_status_text(status));
        return 1;
    }
    source_root = context->project_root;
    (void)memset(built_targets, 0, sizeof(built_targets));
    (void)memset(test_targets, 0, sizeof(test_targets));
    status = umi_build_automation_snapshot(automation, &snapshot);
    if (status != UMI_STATUS_OK) {
        return 1;
    }
    if ((snapshot.actions & UMI_BUILD_AUTOMATION_ACTION_CONFIGURE) != 0U &&
        automation_run_action(context,
                              UMI_BUILD_CONFIGURE,
                              source_root,
                              preset,
                              build_directory,
                              NULL,
                              NULL,
                              NULL,
                              NULL,
                              jobs) != 0) {
        return 1;
    }
    for (index = 0U; index < snapshot.selected_count; ++index) {
        UmiBuildAutomationPlanItem item;

        if (umi_build_automation_item_at(automation, index, &item) !=
            UMI_STATUS_OK) {
            return 1;
        }
        if ((item.actions & UMI_BUILD_AUTOMATION_ACTION_BUILD) == 0U ||
            item.build_target[0] == '\0' || built_all ||
            automation_target_seen(built_targets,
                                   built_count,
                                   item.build_target)) {
            continue;
        }
        if (automation_run_action(context,
                                  UMI_BUILD_COMPILE,
                                  source_root,
                                  preset,
                                  build_directory,
                                  item.build_target,
                                  NULL,
                                  NULL,
                                  NULL,
                                  jobs) != 0) {
            return 1;
        }
        (void)automation_copy_text(built_targets[built_count++],
                                   UMI_BUILD_NAME_CAPACITY,
                                   item.build_target);
        built_all = strcmp(item.build_target, "umicom-products") == 0;
    }
    if (run_tests) {
        for (index = 0U; index < snapshot.selected_count; ++index) {
            UmiBuildAutomationPlanItem item;

            if (umi_build_automation_item_at(automation, index, &item) !=
                UMI_STATUS_OK) {
                return 1;
            }
            if ((item.actions & UMI_BUILD_AUTOMATION_ACTION_TEST) == 0U ||
                item.test_build_target[0] == '\0' || tested_all ||
                automation_target_seen(test_targets,
                                       test_count,
                                       item.test_build_target)) {
                continue;
            }
            if (automation_run_action(context,
                                      UMI_BUILD_COMPILE,
                                      source_root,
                                      preset,
                                      build_directory,
                                      item.test_build_target,
                                      NULL,
                                      NULL,
                                      NULL,
                                      jobs) != 0 ||
                automation_run_action(context,
                                      UMI_BUILD_TEST,
                                      source_root,
                                      preset,
                                      build_directory,
                                      NULL,
                                      item.test_expression,
                                      NULL,
                                      NULL,
                                      jobs) != 0) {
                return 1;
            }
            (void)automation_copy_text(test_targets[test_count++],
                                       UMI_BUILD_NAME_CAPACITY,
                                       item.test_build_target);
            tested_all = strcmp(item.test_build_target, "umicom-tests") == 0;
        }
    }
    if (deploy &&
        (snapshot.actions & (UMI_BUILD_AUTOMATION_ACTION_BUILD |
                             UMI_BUILD_AUTOMATION_ACTION_STAGE)) != 0U) {
        return automation_run_action(context,
                                     UMI_BUILD_INSTALL,
                                     source_root,
                                     preset,
                                     build_directory,
                                     NULL,
                                     NULL,
                                     install_prefix,
                                     NULL,
                                     jobs);
    }
    return 0;
}

/* Create the local durable update channel used by running applications. */
static UmiStatus automation_create_update_broker(
    const char *source_root,
    UmiChangeBroker **out_broker,
    char *out_journal_path,
    size_t journal_capacity)
{
    UmiChangeBrokerConfig config = umi_change_broker_config_default();
    char local_root[UMI_PATH_CAPACITY];
    char runtime_root[UMI_PATH_CAPACITY];
    UmiStatus status;

    if (source_root == NULL || out_broker == NULL ||
        out_journal_path == NULL || journal_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_broker = NULL;
    status = umi_fs_join(local_root,
                         sizeof(local_root),
                         source_root,
                         ".umicom");
    if (status == UMI_STATUS_OK) {
        status = umi_fs_join(runtime_root,
                             sizeof(runtime_root),
                             local_root,
                             "runtime");
    }
    if (status == UMI_STATUS_OK) {
        status = umi_fs_make_directories(runtime_root);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_fs_join(out_journal_path,
                             journal_capacity,
                             runtime_root,
                             "module-updates.journal");
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }
    config.journal_path = out_journal_path;
    config.source_id = "umicom.automated-build";
    return umi_change_broker_create(&config, out_broker);
}

/* Run one workspace watcher until Ctrl+C or an optional first generation. */
static int automation_watch(UmiCliContext *context,
                            const char *source_root,
                            const char *preset,
                            const char *build_directory,
                            const char *install_prefix,
                            int jobs,
                            int run_tests,
                            int deploy,
                            uint32_t debounce_ms,
                            uint32_t poll_ms,
                            int seed_existing_changes,
                            int stop_after_generation,
                            int force_initial_generation)
{
    UmiChangeBroker *broker = NULL;
    UmiBuildContinuousIntegration *continuous = NULL;
    UmiWatcher *watcher = NULL;
    UmiCliAutomationPublication publication;
    UmiCliAutomationWatch watch;
    UmiBuildContinuousConfig continuous_config =
        umi_build_continuous_config_default();
    UmiWatcherConfig watcher_config;
    char journal_path[UMI_PATH_CAPACITY];
    int result = 0;
    UmiStatus status;

    (void)memset(&publication, 0, sizeof(publication));
    (void)memset(&watch, 0, sizeof(watch));
    watch.clock = umi_clock_system();
    watch.event_status = automation_copy_text(
        watch.source_root, sizeof(watch.source_root), source_root);
    if (watch.event_status != UMI_STATUS_OK) {
        return 1;
    }
    status = automation_create_update_broker(source_root,
                                             &broker,
                                             journal_path,
                                             sizeof(journal_path));
    publication.broker = broker;
    continuous_config.debounce_ms = debounce_ms;
    continuous_config.initial_generation =
        umi_change_broker_last_sequence(broker);
    continuous_config.update_sink = automation_publish_update;
    continuous_config.update_user_data = &publication;
    continuous_config.require_update_record = 1;
    if (status == UMI_STATUS_OK) {
        status = umi_build_continuous_create(
            &continuous_config, &continuous);
    }
    watch.continuous = continuous;
    if (status == UMI_STATUS_OK) {
        status = automation_register_foundation_scopes(
            umi_build_continuous_planner(continuous));
    }
    if (status == UMI_STATUS_OK) {
        status = automation_register_application_scopes(
            umi_build_continuous_planner(continuous), source_root);
    }
    watcher_config = umi_watcher_config_default(source_root);
    watcher_config.polling_interval_ms = poll_ms;
    watcher_config.maximum_entries = 131072U;
    watcher_config.sink = automation_watch_event_sink;
    watcher_config.sink_user_data = &watch;
    if (status == UMI_STATUS_OK) {
        status = umi_watcher_create(&watcher_config, &watcher);
    }
    if (status == UMI_STATUS_OK) {
        /* The first polling scan establishes a baseline. Its created events
         * describe files that already exist, not edits made by the user. */
        watch.priming = 1;
        status = umi_watcher_scan_once(watcher);
        watch.priming = 0;
    }
    if (status == UMI_STATUS_OK && force_initial_generation) {
        status = umi_build_continuous_record_change(
            continuous,
            "CMakeLists.txt",
            0,
            automation_clock_milliseconds(&watch.clock, 0));
    } else if (status == UMI_STATUS_OK && seed_existing_changes) {
        /* Existing uncommitted work is seeded once so edits made before the
         * watcher started are not silently left without verification. */
        status = automation_visit_repository_changes(
            source_root, automation_continuous_change_sink, &watch);
    }
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "Unable to start continuous integration: %s\n",
                      umi_status_text(status));
        umi_watcher_destroy(watcher);
        umi_build_continuous_destroy(continuous);
        umi_change_broker_destroy(broker);
        return 1;
    }

    automation_stop_requested = 0;
    (void)signal(SIGINT, automation_stop_handler);
    (void)signal(SIGTERM, automation_stop_handler);
    (void)printf(
        "Watching %s\n"
        "Affected modules will build and test after %u ms without edits.\n"
        "Module update notices are recorded in %s\n"
        "Press Ctrl+C to stop after the current safe operation.\n",
        source_root,
        debounce_ms,
        journal_path);

    while (!automation_stop_requested) {
        const uint64_t now_ms =
            automation_clock_milliseconds(&watch.clock, 0);

        status = umi_watcher_scan_once(watcher);
        if (status == UMI_STATUS_OK) {
            status = watch.event_status;
        }
        if (status != UMI_STATUS_OK) {
            (void)fprintf(stderr,
                          "Source watcher failed: %s\n",
                          umi_status_text(status));
            result = 1;
            break;
        }
        if (umi_build_continuous_ready(continuous, now_ms)) {
            int generation_succeeded;
            int generation_started = 0;
            UmiBuildContinuousSnapshot snapshot;
            UmiStatus generation_status;

            status = umi_build_continuous_begin(continuous, now_ms);
            generation_started = status == UMI_STATUS_OK;
            if (status == UMI_STATUS_OK) {
                status = automation_print_plan(
                    umi_build_continuous_planner(continuous));
            }
            generation_succeeded = status == UMI_STATUS_OK &&
                automation_execute(
                    context,
                    umi_build_continuous_planner(continuous),
                    source_root,
                    preset,
                    build_directory,
                    install_prefix,
                    jobs,
                    run_tests,
                    deploy) == 0;
            generation_status = status;
            if (generation_started) {
                const UmiStatus completion_status =
                    umi_build_continuous_complete(
                    continuous,
                    generation_succeeded,
                    automation_clock_milliseconds(&watch.clock, 0),
                    automation_clock_milliseconds(&watch.clock, 1));

                /* A planning diagnostic remains the primary failure; otherwise
                 * report publication or finalisation failures to the caller. */
                if (generation_status == UMI_STATUS_OK) {
                    generation_status = completion_status;
                }
            }
            if (umi_build_continuous_snapshot(
                    continuous, &snapshot) == UMI_STATUS_OK) {
                (void)printf(
                    "Generation %llu %s: %zu files, %zu affected scopes.\n",
                    (unsigned long long)snapshot.generation,
                    umi_build_continuous_phase_text(snapshot.phase),
                    snapshot.batch_change_count,
                    snapshot.batch_scope_count);
            }
            if (!generation_succeeded ||
                generation_status != UMI_STATUS_OK) {
                result = 1;
            }
            if (stop_after_generation) {
                break;
            }
        }
        if (watch.clock.sleep_milliseconds != NULL) {
            (void)watch.clock.sleep_milliseconds(&watch.clock, poll_ms);
        }
    }

    (void)signal(SIGINT, SIG_DFL);
    (void)signal(SIGTERM, SIG_DFL);
    umi_watcher_destroy(watcher);
    umi_build_continuous_destroy(continuous);
    umi_change_broker_destroy(broker);
    return result;
}

/* Show command-specific help without requiring toolchain or repository discovery. */
static void automation_print_help(void)
{
    (void)puts(
        "Umicom Automated Build System\n\n"
        "Usage:\n"
        "  umicom automate plan [PATH] [--all]\n"
        "  umicom automate run [PATH] [--preset NAME | --build PATH] [--jobs N]\n"
        "                      [--no-tests] [--deploy] [--prefix PATH] [--all]\n"
        "  umicom automate watch [PATH] [--preset NAME | --build PATH] [--jobs N]\n"
        "                      [--debounce MS] [--interval MS] [--no-tests]\n"
        "                      [--deploy] [--prefix PATH] [--ignore-existing]\n\n"
        "plan discovers changes and prints actions without building anything.\n"
        "run builds affected product targets and their focused tests.\n"
        "watch continuously discovers edits, builds and tests affected modules,\n"
        "then records versioned update notices for running applications.\n"
        "--deploy installs only after successful build and test work.\n"
        "--ignore-existing watches only edits made after the watcher starts.\n"
        "--all requests a complete product and test plan even with a clean tree.\n");
}

/* Plan, execute once or continuously verify target-free workspace updates. */
int umi_cli_command_automation(UmiCliContext *context, int argc, char **argv)
{
    UmiBuildAutomation *automation = NULL;
    UmiBuildAutomationChange forced_change;
    UmiBuildAutomationSnapshot snapshot;
    const char *mode = argc > 0 && argv[0][0] != '-' ? argv[0] : "plan";
    const char *source_option = automation_option_value(argc, argv, "--source");
    const char *preset = automation_option_value(argc, argv, "--preset");
    const char *build_directory = automation_option_value(argc, argv, "--build");
    const char *install_prefix = automation_option_value(argc, argv, "--prefix");
    const char *jobs_text = automation_option_value(argc, argv, "--jobs");
    const char *debounce_text =
        automation_option_value(argc, argv, "--debounce");
    const char *interval_text =
        automation_option_value(argc, argv, "--interval");
    const char *source_root = source_option;
    uint32_t debounce_ms = 750U;
    uint32_t poll_ms = 500U;
    int jobs = 0;
    int result = 1;
    UmiStatus status;

    if (strcmp(mode, "help") == 0 || strcmp(mode, "--help") == 0 ||
        automation_has_flag(argc, argv, "--help")) {
        automation_print_help();
        return 0;
    }
    if (strcmp(mode, "plan") != 0 && strcmp(mode, "run") != 0 &&
        strcmp(mode, "watch") != 0) {
        (void)fprintf(stderr, "Unknown automated build command: %s\n", mode);
        automation_print_help();
        return 2;
    }
    if (source_root == NULL && argc > 1 && argv[1][0] != '-') {
        source_root = argv[1];
    }
    status = automation_parse_jobs(jobs_text, &jobs);
    if (status != UMI_STATUS_OK) {
        (void)fputs("--jobs requires a whole number from zero to INT_MAX.\n",
                    stderr);
        return 2;
    }
    status = automation_parse_milliseconds(
        debounce_text, 750U, &debounce_ms);
    if (status == UMI_STATUS_OK) {
        status = automation_parse_milliseconds(
            interval_text, 500U, &poll_ms);
    }
    if (status != UMI_STATUS_OK) {
        (void)fputs(
            "--debounce and --interval require positive whole milliseconds.\n",
            stderr);
        return 2;
    }
    /* Planning reads repository state only. Compiler discovery is deferred
     * until run mode has a non-empty plan, so a read-only plan cannot start a
     * compiler probe or require an installed build toolchain. */
    status = umi_cli_context_prepare_operation(
        context,
        source_root,
        UMI_TOOLCHAIN_OPERATION_REPOSITORY_READ);
    if (status != UMI_STATUS_OK) {
        return 1;
    }
    status = automation_resolve_project_root(context);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "Unable to resolve workspace path: %s\n",
                      umi_status_text(status));
        return 1;
    }
    source_root = context->project_root;
#ifdef _WIN32
    if (preset == NULL && build_directory == NULL) {
        preset = "windows-ucrt64-headless-debug";
    }
#else
    if (preset == NULL && build_directory == NULL) {
        preset = "headless-debug";
    }
#endif
    if (strcmp(mode, "watch") == 0) {
        return automation_watch(
            context,
            source_root,
            preset,
            build_directory,
            install_prefix,
            jobs,
            !automation_has_flag(argc, argv, "--no-tests"),
            automation_has_flag(argc, argv, "--deploy"),
            debounce_ms,
            poll_ms,
            !automation_has_flag(argc, argv, "--ignore-existing"),
            automation_has_flag(argc, argv, "--once"),
            automation_has_flag(argc, argv, "--all"));
    }
    status = umi_build_automation_create(&automation);
    if (status == UMI_STATUS_OK) {
        status = automation_register_foundation_scopes(automation);
    }
    if (status == UMI_STATUS_OK) {
        status = automation_register_application_scopes(
            automation, source_root);
    }
    if (status == UMI_STATUS_OK && automation_has_flag(argc, argv, "--all")) {
        umi_build_automation_change_init(
            &forced_change, "CMakeLists.txt", 0);
        status = umi_build_automation_add_change(automation, &forced_change);
    } else if (status == UMI_STATUS_OK) {
        status = automation_discover_changes(automation, source_root);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_build_automation_evaluate(automation);
    }
    if (status == UMI_STATUS_OK) {
        status = automation_print_plan(automation);
    }
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "Unable to create automated build plan: %s\n",
                      umi_status_text(status));
        umi_build_automation_destroy(automation);
        return 1;
    }
    status = umi_build_automation_snapshot(automation, &snapshot);
    if (status != UMI_STATUS_OK || snapshot.change_count == 0U) {
        (void)puts("No changed files require build, test or deployment work.");
        umi_build_automation_destroy(automation);
        return status == UMI_STATUS_OK ? 0 : 1;
    }
    if (strcmp(mode, "plan") == 0) {
        umi_build_automation_destroy(automation);
        return 0;
    }
    result = automation_execute(
        context,
        automation,
        source_root,
        preset,
        build_directory,
        install_prefix,
        jobs,
        !automation_has_flag(argc, argv, "--no-tests"),
        automation_has_flag(argc, argv, "--deploy"));
    umi_build_automation_destroy(automation);
    return result;
}
