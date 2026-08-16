/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/workspace_import.c
 *
 * PURPOSE:
 *   Implement filesystem-backed project import and project-scoped validation
 *   as a bounded project-platform module.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The importer adds metadata to the public project workspace. It does not run
 * CMake, Git, a compiler or an application. Execution remains the responsibility
 * of the Developer Runtime and its explicit executor boundary.
 */
#include "umicom/project/workspace_import.h"

#include "umicom/platform/directory.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------------- */
/* Filesystem-backed workspace import.                                       */
/*                                                                           */
/* The implementation lives in the project library beside the file-set       */
/* registry because import performs source discovery before it populates the  */
/* workspace's other public registries.  No product-specific state is used.   */
/* ------------------------------------------------------------------------- */

typedef struct UmiProjectImportScan {
    size_t discovered_file_count;
    size_t c_source_count;
    size_t cpp_source_count;
    size_t header_count;
} UmiProjectImportScan;

static void import_copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) memcpy(destination, source, length);
    destination[length] = '\0';
}

static int ascii_lower(int value)
{
    if (value >= 'A' && value <= 'Z') return value - 'A' + 'a';
    return value;
}

static int extension_equals(const char *name, const char *extension)
{
    const char *dot;
    size_t index;
    size_t left_length;
    size_t right_length;

    if (name == NULL || extension == NULL) return 0;
    dot = strrchr(name, '.');
    if (dot == NULL) return 0;
    left_length = strlen(dot);
    right_length = strlen(extension);
    if (left_length != right_length) return 0;
    for (index = 0U; index < left_length; ++index) {
        if (ascii_lower((unsigned char)dot[index]) !=
            ascii_lower((unsigned char)extension[index])) return 0;
    }
    return 1;
}

static UmiStatus import_scan_visitor(const UmiFileInfo *info, void *user_data)
{
    UmiProjectImportScan *scan = (UmiProjectImportScan *)user_data;
    if (info == NULL || scan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (info->kind != UMI_FILE_KIND_REGULAR) return UMI_STATUS_OK;

    scan->discovered_file_count += 1U;
    if (extension_equals(info->name, ".c")) {
        scan->c_source_count += 1U;
    } else if (extension_equals(info->name, ".cc") ||
               extension_equals(info->name, ".cpp") ||
               extension_equals(info->name, ".cxx")) {
        scan->cpp_source_count += 1U;
    } else if (extension_equals(info->name, ".h") ||
               extension_equals(info->name, ".hh") ||
               extension_equals(info->name, ".hpp") ||
               extension_equals(info->name, ".hxx")) {
        scan->header_count += 1U;
    }
    return UMI_STATUS_OK;
}

static UmiStatus resolve_import_root(
    const char *root_directory,
    char *out_root,
    size_t capacity)
{
    char current_directory[UMI_PATH_CAPACITY];
    UmiStatus status;

    if (root_directory == NULL || root_directory[0] == '\0' ||
        out_root == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;

    if (umi_path_is_absolute(root_directory)) {
        status = umi_path_normalise(root_directory, out_root, capacity);
    } else {
        status = umi_fs_current_directory(current_directory,
                                          sizeof(current_directory));
        if (status != UMI_STATUS_OK) return status;
        status = umi_path_absolute(root_directory, current_directory,
                                   out_root, capacity);
    }
    if (status != UMI_STATUS_OK) return status;
    return umi_fs_is_directory(out_root) ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

static UmiStatus resolve_import_build_directory(
    const UmiProjectWorkspaceImportRequest *request,
    const char *root_directory,
    char *out_build,
    size_t capacity)
{
    if (request->build_directory == NULL ||
        request->build_directory[0] == '\0') {
        return umi_path_join(root_directory, "build", out_build, capacity);
    }
    if (umi_path_is_absolute(request->build_directory)) {
        return umi_path_normalise(request->build_directory,
                                  out_build, capacity);
    }
    return umi_path_absolute(request->build_directory, root_directory,
                             out_build, capacity);
}

static UmiStatus make_project_id(
    const char *requested_id,
    const char *root_directory,
    char *out_id,
    size_t capacity)
{
    char basename[UMI_PATH_CAPACITY];
    const char *source;
    size_t source_index;
    size_t destination_index = 0U;
    UmiStatus status;

    if (out_id == NULL || capacity < 2U) return UMI_STATUS_INVALID_ARGUMENT;
    source = requested_id;
    if (source == NULL || source[0] == '\0') {
        status = umi_path_basename(root_directory, basename, sizeof(basename));
        if (status != UMI_STATUS_OK) return status;
        source = basename;
    }

    for (source_index = 0U; source[source_index] != '\0'; ++source_index) {
        unsigned char ch = (unsigned char)source[source_index];
        char value;
        if ((ch >= (unsigned char)'a' && ch <= (unsigned char)'z') ||
            (ch >= (unsigned char)'A' && ch <= (unsigned char)'Z') ||
            (ch >= (unsigned char)'0' && ch <= (unsigned char)'9') ||
            ch == (unsigned char)'_' || ch == (unsigned char)'-' ||
            ch == (unsigned char)'.') {
            value = (char)ch;
        } else {
            value = '-';
        }
        if (destination_index + 1U >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        if (value == '-' && destination_index > 0U &&
            out_id[destination_index - 1U] == '-') continue;
        out_id[destination_index++] = value;
    }
    while (destination_index > 0U && out_id[destination_index - 1U] == '-')
        destination_index -= 1U;
    if (destination_index == 0U) {
        import_copy_text(out_id, capacity, "project");
    } else {
        out_id[destination_index] = '\0';
    }
    return UMI_STATUS_OK;
}

static UmiStatus make_record_id(
    const char *project_id,
    const char *suffix,
    char *out_id,
    size_t capacity)
{
    int written;
    if (project_id == NULL || suffix == NULL || out_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    written = snprintf(out_id, capacity, "%s.%s", project_id, suffix);
    if (written < 0 || (size_t)written >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

static int safe_command_component(const char *text)
{
    const unsigned char *cursor = (const unsigned char *)text;
    if (text == NULL) return 1;
    while (*cursor != 0U) {
        if (*cursor == (unsigned char)'"' || *cursor == (unsigned char)'\n' ||
            *cursor == (unsigned char)'\r' || *cursor == (unsigned char)'|' ||
            *cursor == (unsigned char)'&' || *cursor == (unsigned char)';' ||
            *cursor == (unsigned char)'<' || *cursor == (unsigned char)'>' ||
            *cursor == (unsigned char)'`' || *cursor == (unsigned char)'(' ||
            *cursor == (unsigned char)')') return 0;
        ++cursor;
    }
    return 1;
}

static const char *import_configuration_name(
    const UmiProjectWorkspaceImportRequest *request)
{
    return request->configuration_name != NULL &&
           request->configuration_name[0] != '\0'
               ? request->configuration_name : "Debug";
}

static const char *import_generator(
    const UmiProjectWorkspaceImportRequest *request)
{
    return request->generator != NULL && request->generator[0] != '\0'
               ? request->generator : "Ninja";
}

static const char *import_toolchain_id(
    const UmiProjectWorkspaceImportRequest *request)
{
    return request->toolchain_id != NULL && request->toolchain_id[0] != '\0'
               ? request->toolchain_id : "default";
}

static UmiStatus upsert_import_task(
    UmiProjectWorkspace *workspace,
    const char *id,
    const char *project_id,
    const char *label,
    const char *command,
    const char *working_directory,
    const char *group,
    int default_task)
{
    UmiProjectTaskSnapshot task;
    memset(&task, 0, sizeof(task));
    task.struct_size = (uint32_t)sizeof(task);
    task.api_version = UMI_PROJECT_TASK_API_VERSION;
    import_copy_text(task.id, sizeof(task.id), id);
    import_copy_text(task.project_id, sizeof(task.project_id), project_id);
    import_copy_text(task.label, sizeof(task.label), label);
    import_copy_text(task.command, sizeof(task.command), command);
    import_copy_text(task.working_directory, sizeof(task.working_directory),
                     working_directory);
    import_copy_text(task.group, sizeof(task.group), group);
    task.default_task = default_task;
    task.background = 0;
    task.enabled = 1;
    return umi_project_task_registry_upsert(
        umi_project_workspace_task(workspace), &task);
}

static UmiStatus upsert_build_node(
    UmiProjectWorkspace *workspace,
    const char *id,
    const char *project_id,
    const char *target_id,
    const char *label,
    const char *kind,
    const char *depends_on,
    int32_t order)
{
    UmiProjectBuildNodeSnapshot node;
    memset(&node, 0, sizeof(node));
    node.struct_size = (uint32_t)sizeof(node);
    node.api_version = UMI_PROJECT_BUILD_NODE_API_VERSION;
    import_copy_text(node.id, sizeof(node.id), id);
    import_copy_text(node.project_id, sizeof(node.project_id), project_id);
    import_copy_text(node.target_id, sizeof(node.target_id), target_id);
    import_copy_text(node.label, sizeof(node.label), label);
    import_copy_text(node.kind, sizeof(node.kind), kind);
    import_copy_text(node.depends_on, sizeof(node.depends_on), depends_on);
    node.state = 0;
    node.order = order;
    return umi_project_build_node_registry_upsert(
        umi_project_workspace_build_node(workspace), &node);
}

UmiStatus umi_project_workspace_import_directory(
    UmiProjectWorkspace *workspace,
    const UmiProjectWorkspaceImportRequest *request,
    UmiProjectWorkspaceImportSnapshot *out_snapshot)
{
    UmiProjectImportScan scan;
    UmiDirectoryWalkOptions walk_options;
    UmiProjectDescriptorSnapshot descriptor;
    UmiProjectConfigurationSnapshot configuration;
    UmiProjectTargetSnapshot target;
    UmiProjectEnvironmentSnapshot environment;
    UmiProjectFileSetSnapshot file_set;
    UmiProjectLaunchProfileSnapshot launch;
    UmiProjectWorkspaceSelectionRequest selection_request;
    char cmake_path[UMI_PATH_CAPACITY];
    char git_path[UMI_PATH_CAPACITY];
    char source_set_id[128];
    char configure_command[1024];
    char build_command[1024];
    char test_command[1024];
    char configure_node_id[128];
    char build_node_id[128];
    char test_node_id[128];
    char run_node_id[128];
    const char *configuration_name;
    const char *generator;
    const char *toolchain_id;
    uint32_t parallel_jobs;
    UmiStatus status;
    int written;

    if (workspace == NULL || request == NULL || out_snapshot == NULL ||
        request->root_directory == NULL || request->root_directory[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;

#define snapshot (*out_snapshot)
    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.struct_size = (uint32_t)sizeof(snapshot);
    snapshot.api_version = UMI_PROJECT_WORKSPACE_IMPORT_API_VERSION;
    memset(&scan, 0, sizeof(scan));

    status = resolve_import_root(request->root_directory,
                                 snapshot.root_directory,
                                 sizeof(snapshot.root_directory));
    if (status != UMI_STATUS_OK) return status;
    status = resolve_import_build_directory(request, snapshot.root_directory,
                                             snapshot.build_directory,
                                             sizeof(snapshot.build_directory));
    if (status != UMI_STATUS_OK) return status;

    /*
     * Project record fields intentionally use a smaller stable ABI capacity
     * than the generic path helper.  Reject an unrepresentable path instead
     * of silently truncating it while creating project metadata.
     */
    if (strlen(snapshot.root_directory) >= 1024U ||
        strlen(snapshot.build_directory) >= 1024U)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    status = make_project_id(request->project_id, snapshot.root_directory,
                             snapshot.project_id, sizeof(snapshot.project_id));
    if (status != UMI_STATUS_OK) return status;

    if (request->display_name != NULL && request->display_name[0] != '\0') {
        if (strlen(request->display_name) >= sizeof(snapshot.display_name))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        import_copy_text(snapshot.display_name, sizeof(snapshot.display_name),
                         request->display_name);
    } else {
        char name[UMI_PATH_CAPACITY];
        status = umi_path_basename(snapshot.root_directory, name, sizeof(name));
        if (status != UMI_STATUS_OK) return status;
        if (strlen(name) >= sizeof(snapshot.display_name))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        import_copy_text(snapshot.display_name, sizeof(snapshot.display_name), name);
    }

    status = umi_path_join(snapshot.root_directory, "CMakeLists.txt",
                           cmake_path, sizeof(cmake_path));
    if (status != UMI_STATUS_OK) return status;
    snapshot.has_cmake = umi_fs_is_file(cmake_path);
    status = umi_path_join(snapshot.root_directory, ".git",
                           git_path, sizeof(git_path));
    if (status != UMI_STATUS_OK) return status;
    snapshot.has_git = umi_fs_is_directory(git_path);

    walk_options = umi_directory_walk_options_default();
    walk_options.recursive = 1;
    walk_options.max_depth = 64U;
    walk_options.include_files = 1;
    walk_options.include_directories = 0;
    walk_options.include_hidden = 0;
    walk_options.follow_symbolic_links = 0;
    status = umi_directory_walk(snapshot.root_directory, &walk_options,
                                import_scan_visitor, &scan);
    if (status != UMI_STATUS_OK) return status;
    snapshot.discovered_file_count = scan.discovered_file_count;
    snapshot.c_source_count = scan.c_source_count;
    snapshot.cpp_source_count = scan.cpp_source_count;
    snapshot.header_count = scan.header_count;

    configuration_name = import_configuration_name(request);
    generator = import_generator(request);
    toolchain_id = import_toolchain_id(request);
    if (strlen(configuration_name) >= sizeof(configuration.build_type) ||
        strlen(toolchain_id) >= sizeof(configuration.toolchain_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    parallel_jobs = request->parallel_jobs != 0U
                        ? request->parallel_jobs
                        : UMI_PROJECT_WORKSPACE_IMPORT_DEFAULT_PARALLEL_JOBS;
    if (!safe_command_component(snapshot.root_directory) ||
        !safe_command_component(snapshot.build_directory) ||
        !safe_command_component(configuration_name) ||
        !safe_command_component(generator)) return UMI_STATUS_INVALID_ARGUMENT;

    memset(&descriptor, 0, sizeof(descriptor));
    descriptor.struct_size = (uint32_t)sizeof(descriptor);
    descriptor.api_version = UMI_PROJECT_DESCRIPTOR_API_VERSION;
    import_copy_text(descriptor.id, sizeof(descriptor.id), snapshot.project_id);
    import_copy_text(descriptor.name, sizeof(descriptor.name), snapshot.display_name);
    import_copy_text(descriptor.root_uri, sizeof(descriptor.root_uri),
                     snapshot.root_directory);
    import_copy_text(descriptor.kind, sizeof(descriptor.kind),
                     snapshot.has_cmake ? "cmake" : "native");
    if (scan.c_source_count > 0U && scan.cpp_source_count > 0U)
        import_copy_text(descriptor.primary_language,
                         sizeof(descriptor.primary_language), "C/C++");
    else if (scan.cpp_source_count > 0U)
        import_copy_text(descriptor.primary_language,
                         sizeof(descriptor.primary_language), "C++");
    else
        import_copy_text(descriptor.primary_language,
                         sizeof(descriptor.primary_language), "C");
    import_copy_text(descriptor.description, sizeof(descriptor.description),
                     "Imported from an existing source directory.");
    descriptor.enabled = 1;
    status = umi_project_descriptor_registry_upsert(
        umi_project_workspace_descriptor(workspace), &descriptor);
    if (status != UMI_STATUS_OK) return status;
    snapshot.created_or_updated_record_count += 1U;

    status = make_record_id(snapshot.project_id, "configuration",
                            snapshot.configuration_id,
                            sizeof(snapshot.configuration_id));
    if (status != UMI_STATUS_OK) return status;
    memset(&configuration, 0, sizeof(configuration));
    configuration.struct_size = (uint32_t)sizeof(configuration);
    configuration.api_version = UMI_PROJECT_CONFIGURATION_API_VERSION;
    import_copy_text(configuration.id, sizeof(configuration.id),
                     snapshot.configuration_id);
    import_copy_text(configuration.project_id, sizeof(configuration.project_id),
                     snapshot.project_id);
    import_copy_text(configuration.name, sizeof(configuration.name),
                     configuration_name);
    import_copy_text(configuration.build_type, sizeof(configuration.build_type),
                     configuration_name);
    import_copy_text(configuration.toolchain_id,
                     sizeof(configuration.toolchain_id), toolchain_id);
#if defined(_WIN32)
    import_copy_text(configuration.platform, sizeof(configuration.platform),
                     "windows");
#elif defined(__APPLE__)
    import_copy_text(configuration.platform, sizeof(configuration.platform),
                     "macos");
#else
    import_copy_text(configuration.platform, sizeof(configuration.platform),
                     "linux");
#endif
    configuration.active = 1;
    status = umi_project_configuration_registry_upsert(
        umi_project_workspace_configuration(workspace), &configuration);
    if (status != UMI_STATUS_OK) return status;
    snapshot.created_or_updated_record_count += 1U;

    status = make_record_id(snapshot.project_id, "target",
                            snapshot.target_id, sizeof(snapshot.target_id));
    if (status != UMI_STATUS_OK) return status;
    memset(&target, 0, sizeof(target));
    target.struct_size = (uint32_t)sizeof(target);
    target.api_version = UMI_PROJECT_TARGET_API_VERSION;
    import_copy_text(target.id, sizeof(target.id), snapshot.target_id);
    import_copy_text(target.project_id, sizeof(target.project_id), snapshot.project_id);
    import_copy_text(target.name, sizeof(target.name), "Default Target");
    import_copy_text(target.kind, sizeof(target.kind),
                     snapshot.has_cmake ? "cmake" : "native");
    import_copy_text(target.output_uri, sizeof(target.output_uri),
                     snapshot.build_directory);
    target.enabled = 1;
    target.default_target = 1;
    status = umi_project_target_registry_upsert(
        umi_project_workspace_target(workspace), &target);
    if (status != UMI_STATUS_OK) return status;
    snapshot.created_or_updated_record_count += 1U;

    status = make_record_id(snapshot.project_id, "environment",
                            snapshot.environment_id,
                            sizeof(snapshot.environment_id));
    if (status != UMI_STATUS_OK) return status;
    memset(&environment, 0, sizeof(environment));
    environment.struct_size = (uint32_t)sizeof(environment);
    environment.api_version = UMI_PROJECT_ENVIRONMENT_API_VERSION;
    import_copy_text(environment.id, sizeof(environment.id), snapshot.environment_id);
    import_copy_text(environment.project_id, sizeof(environment.project_id),
                     snapshot.project_id);
    import_copy_text(environment.name, sizeof(environment.name),
                     "Inherited Development Environment");
    import_copy_text(environment.toolchain_id, sizeof(environment.toolchain_id),
                     toolchain_id);
    environment.inherit_parent = 1;
    status = umi_project_environment_registry_upsert(
        umi_project_workspace_environment(workspace), &environment);
    if (status != UMI_STATUS_OK) return status;
    snapshot.created_or_updated_record_count += 1U;

    status = make_record_id(snapshot.project_id, "sources",
                            source_set_id, sizeof(source_set_id));
    if (status != UMI_STATUS_OK) return status;
    memset(&file_set, 0, sizeof(file_set));
    file_set.struct_size = (uint32_t)sizeof(file_set);
    file_set.api_version = UMI_PROJECT_FILE_SET_API_VERSION;
    import_copy_text(file_set.id, sizeof(file_set.id), source_set_id);
    import_copy_text(file_set.project_id, sizeof(file_set.project_id),
                     snapshot.project_id);
    import_copy_text(file_set.name, sizeof(file_set.name), "Source Tree");
    import_copy_text(file_set.root_uri, sizeof(file_set.root_uri),
                     snapshot.root_directory);
    import_copy_text(file_set.include_glob, sizeof(file_set.include_glob), "**/*");
    import_copy_text(file_set.exclude_glob, sizeof(file_set.exclude_glob),
                     "build/**;.git/**");
    file_set.file_count = scan.discovered_file_count;
    file_set.generated = 0;
    status = umi_project_file_set_registry_upsert(
        umi_project_workspace_file_set(workspace), &file_set);
    if (status != UMI_STATUS_OK) return status;
    snapshot.created_or_updated_record_count += 1U;

    if (snapshot.has_cmake) {
        status = make_record_id(snapshot.project_id, "task.configure",
                                snapshot.configure_task_id,
                                sizeof(snapshot.configure_task_id));
        if (status != UMI_STATUS_OK) return status;
        status = make_record_id(snapshot.project_id, "task.build",
                                snapshot.build_task_id,
                                sizeof(snapshot.build_task_id));
        if (status != UMI_STATUS_OK) return status;
        status = make_record_id(snapshot.project_id, "task.test",
                                snapshot.test_task_id,
                                sizeof(snapshot.test_task_id));
        if (status != UMI_STATUS_OK) return status;

        written = snprintf(configure_command, sizeof(configure_command),
                           "cmake -S \"%s\" -B \"%s\" -G \"%s\" -DCMAKE_BUILD_TYPE=%s",
                           snapshot.root_directory, snapshot.build_directory,
                           generator, configuration_name);
        if (written < 0 || (size_t)written >= sizeof(configure_command))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        written = snprintf(build_command, sizeof(build_command),
                           "cmake --build \"%s\" --parallel %u",
                           snapshot.build_directory, (unsigned int)parallel_jobs);
        if (written < 0 || (size_t)written >= sizeof(build_command))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        written = snprintf(test_command, sizeof(test_command),
                           "ctest --test-dir \"%s\" --output-on-failure -C %s",
                           snapshot.build_directory, configuration_name);
        if (written < 0 || (size_t)written >= sizeof(test_command))
            return UMI_STATUS_CAPACITY_EXCEEDED;

        status = upsert_import_task(workspace, snapshot.configure_task_id,
                                    snapshot.project_id, "Configure", configure_command,
                                    snapshot.root_directory, "configure", 0);
        if (status != UMI_STATUS_OK) return status;
        snapshot.created_or_updated_record_count += 1U;
        status = upsert_import_task(workspace, snapshot.build_task_id,
                                    snapshot.project_id, "Build", build_command,
                                    snapshot.root_directory, "build", 1);
        if (status != UMI_STATUS_OK) return status;
        snapshot.created_or_updated_record_count += 1U;
        if (request->create_test_task != 0) {
            status = upsert_import_task(workspace, snapshot.test_task_id,
                                        snapshot.project_id, "Test", test_command,
                                        snapshot.root_directory, "test", 0);
            if (status != UMI_STATUS_OK) return status;
            snapshot.created_test_task = 1;
            snapshot.created_or_updated_record_count += 1U;
        } else {
            snapshot.test_task_id[0] = '\0';
        }

        status = make_record_id(snapshot.project_id, "node.configure",
                                configure_node_id, sizeof(configure_node_id));
        if (status != UMI_STATUS_OK) return status;
        status = make_record_id(snapshot.project_id, "node.build",
                                build_node_id, sizeof(build_node_id));
        if (status != UMI_STATUS_OK) return status;
        status = make_record_id(snapshot.project_id, "node.test",
                                test_node_id, sizeof(test_node_id));
        if (status != UMI_STATUS_OK) return status;
        status = upsert_build_node(workspace, configure_node_id, snapshot.project_id,
                                   snapshot.target_id, "Configure", "configure", "", 10);
        if (status != UMI_STATUS_OK) return status;
        snapshot.created_or_updated_record_count += 1U;
        status = upsert_build_node(workspace, build_node_id, snapshot.project_id,
                                   snapshot.target_id, "Build", "build",
                                   configure_node_id, 20);
        if (status != UMI_STATUS_OK) return status;
        snapshot.created_or_updated_record_count += 1U;
        if (snapshot.created_test_task) {
            status = upsert_build_node(workspace, test_node_id, snapshot.project_id,
                                       snapshot.target_id, "Test", "test",
                                       build_node_id, 30);
            if (status != UMI_STATUS_OK) return status;
            snapshot.created_or_updated_record_count += 1U;
        }
    }

    if (request->launch_program != NULL && request->launch_program[0] != '\0') {
        if (strlen(request->launch_program) >= sizeof(launch.program) ||
            (request->launch_arguments != NULL &&
             strlen(request->launch_arguments) >= sizeof(launch.arguments)))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        if (!safe_command_component(request->launch_program) ||
            !safe_command_component(request->launch_arguments))
            return UMI_STATUS_INVALID_ARGUMENT;
        status = make_record_id(snapshot.project_id, "run",
                                snapshot.launch_profile_id,
                                sizeof(snapshot.launch_profile_id));
        if (status != UMI_STATUS_OK) return status;
        memset(&launch, 0, sizeof(launch));
        launch.struct_size = (uint32_t)sizeof(launch);
        launch.api_version = UMI_PROJECT_LAUNCH_PROFILE_API_VERSION;
        import_copy_text(launch.id, sizeof(launch.id), snapshot.launch_profile_id);
        import_copy_text(launch.project_id, sizeof(launch.project_id), snapshot.project_id);
        import_copy_text(launch.name, sizeof(launch.name), "Run");
        import_copy_text(launch.program, sizeof(launch.program), request->launch_program);
        import_copy_text(launch.arguments, sizeof(launch.arguments),
                         request->launch_arguments);
        import_copy_text(launch.working_directory, sizeof(launch.working_directory),
                         snapshot.build_directory);
        import_copy_text(launch.environment_id, sizeof(launch.environment_id),
                         snapshot.environment_id);
        launch.debug = 0;
        launch.default_profile = 1;
        status = umi_project_launch_profile_registry_upsert(
            umi_project_workspace_launch_profile(workspace), &launch);
        if (status != UMI_STATUS_OK) return status;
        snapshot.has_launch_profile = 1;
        snapshot.created_or_updated_record_count += 1U;

        status = make_record_id(snapshot.project_id, "node.run",
                                run_node_id, sizeof(run_node_id));
        if (status != UMI_STATUS_OK) return status;
        status = upsert_build_node(workspace, run_node_id, snapshot.project_id,
                                   snapshot.target_id, "Run", "run",
                                   snapshot.has_cmake ? build_node_id : "", 40);
        if (status != UMI_STATUS_OK) return status;
        snapshot.created_or_updated_record_count += 1U;
    }

    memset(&selection_request, 0, sizeof(selection_request));
    selection_request.struct_size = (uint32_t)sizeof(selection_request);
    selection_request.api_version = UMI_PROJECT_WORKSPACE_QUERY_API_VERSION;
    selection_request.project_id = snapshot.project_id;
    selection_request.configuration_id = snapshot.configuration_id;
    selection_request.target_id = snapshot.target_id;
    selection_request.environment_id = snapshot.environment_id;
    if (snapshot.build_task_id[0] != '\0')
        selection_request.task_id = snapshot.build_task_id;
    if (snapshot.has_launch_profile)
        selection_request.launch_profile_id = snapshot.launch_profile_id;

    status = umi_project_workspace_resolve_selection(
        workspace, &selection_request, &snapshot.selection);
    if (status != UMI_STATUS_OK) return status;
    status = umi_project_workspace_validate_project(
        workspace, snapshot.project_id, &snapshot.validation);
    if (status != UMI_STATUS_OK) return status;

#undef snapshot
    return UMI_STATUS_OK;
}

/* ------------------------------------------------------------------------- */
/* Project-scoped validation.                                                 */
/*                                                                           */
/* Whole-workspace validation remains available for repository health checks. */
/* This scoped form prevents an unrelated broken project from blocking build  */
/* or run operations for a different, valid project in the same workspace.   */
/* ------------------------------------------------------------------------- */

static void scoped_add_issue(
    UmiProjectWorkspaceValidationReport *report,
    UmiProjectWorkspaceIssueSeverity severity,
    const char *code,
    const char *project_id,
    const char *item_id,
    const char *message)
{
    UmiProjectWorkspaceValidationIssue *issue;
    if (report == NULL) return;
    if (severity == UMI_PROJECT_WORKSPACE_ISSUE_ERROR) report->error_count += 1U;
    else if (severity == UMI_PROJECT_WORKSPACE_ISSUE_WARNING)
        report->warning_count += 1U;
    if (report->issue_count >= UMI_PROJECT_WORKSPACE_VALIDATION_ISSUE_CAPACITY)
        return;
    issue = &report->issues[report->issue_count++];
    memset(issue, 0, sizeof(*issue));
    issue->struct_size = (uint32_t)sizeof(*issue);
    issue->api_version = UMI_PROJECT_WORKSPACE_VALIDATION_API_VERSION;
    issue->severity = severity;
    import_copy_text(issue->code, sizeof(issue->code), code);
    import_copy_text(issue->project_id, sizeof(issue->project_id), project_id);
    import_copy_text(issue->item_id, sizeof(issue->item_id), item_id);
    import_copy_text(issue->message, sizeof(issue->message), message);
}

static int scoped_project_id_matches(const char *left, const char *right)
{
    return left != NULL && right != NULL && strcmp(left, right) == 0;
}

UmiStatus umi_project_workspace_validate_project(
    const UmiProjectWorkspace *workspace,
    const char *project_id,
    UmiProjectWorkspaceValidationReport *out_report)
{
    UmiProjectWorkspaceSnapshot workspace_snapshot;
    UmiProjectDescriptorSnapshot descriptor;
    UmiProjectConfigurationSnapshot configuration;
    UmiProjectTargetSnapshot target;
    UmiProjectTaskSnapshot task;
    UmiProjectLaunchProfileSnapshot launch;
    UmiProjectDependencySnapshot dependency;
    UmiProjectReferenceSnapshot reference;
    UmiProjectBuildNodeSnapshot node;
    UmiProjectTargetSnapshot referenced_target;
    UmiProjectDescriptorSnapshot referenced_project;
    size_t index;
    size_t active_configurations = 0U;
    size_t default_targets = 0U;
    size_t default_tasks = 0U;
    size_t default_launch_profiles = 0U;
    size_t configurations = 0U;
    size_t targets = 0U;
    UmiStatus status;

    if (workspace == NULL || project_id == NULL || project_id[0] == '\0' ||
        out_report == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    memset(out_report, 0, sizeof(*out_report));
    out_report->struct_size = (uint32_t)sizeof(*out_report);
    out_report->api_version = UMI_PROJECT_WORKSPACE_VALIDATION_API_VERSION;
    status = umi_project_workspace_snapshot(workspace, &workspace_snapshot);
    if (status != UMI_STATUS_OK) return status;
    out_report->workspace_revision = workspace_snapshot.revision;

    status = umi_project_descriptor_registry_find(
        umi_project_workspace_descriptor((UmiProjectWorkspace *)workspace),
        project_id, &descriptor);
    if (status != UMI_STATUS_OK) {
        scoped_add_issue(out_report, UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                         "project.not-found", project_id, project_id,
                         "Requested project does not exist in the workspace.");
        out_report->valid = 0;
        return UMI_STATUS_OK;
    }
    if (descriptor.enabled == 0) {
        scoped_add_issue(out_report, UMI_PROJECT_WORKSPACE_ISSUE_WARNING,
                         "project.disabled", project_id, project_id,
                         "Project is present but is currently disabled.");
    }

    for (index = 0U; index < umi_project_configuration_registry_count(
             umi_project_workspace_configuration((UmiProjectWorkspace *)workspace));
         ++index) {
        if (umi_project_configuration_registry_at(
                umi_project_workspace_configuration((UmiProjectWorkspace *)workspace),
                index, &configuration) != UMI_STATUS_OK ||
            !scoped_project_id_matches(configuration.project_id, project_id)) continue;
        configurations += 1U;
        if (configuration.active != 0) active_configurations += 1U;
    }
    if (active_configurations > 1U) {
        scoped_add_issue(out_report, UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                         "configuration.multiple-active", project_id, "",
                         "Project contains more than one active configuration.");
    } else if (configurations == 0U) {
        scoped_add_issue(out_report, UMI_PROJECT_WORKSPACE_ISSUE_WARNING,
                         "configuration.none", project_id, "",
                         "Project does not define a build configuration.");
    }

    for (index = 0U; index < umi_project_target_registry_count(
             umi_project_workspace_target((UmiProjectWorkspace *)workspace)); ++index) {
        if (umi_project_target_registry_at(
                umi_project_workspace_target((UmiProjectWorkspace *)workspace),
                index, &target) != UMI_STATUS_OK ||
            !scoped_project_id_matches(target.project_id, project_id)) continue;
        targets += 1U;
        if (target.enabled != 0 && target.default_target != 0) default_targets += 1U;
    }
    if (default_targets > 1U) {
        scoped_add_issue(out_report, UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                         "target.multiple-defaults", project_id, "",
                         "Project contains more than one enabled default target.");
    } else if (targets == 0U) {
        scoped_add_issue(out_report, UMI_PROJECT_WORKSPACE_ISSUE_WARNING,
                         "target.none", project_id, "",
                         "Project does not define a target.");
    }

    for (index = 0U; index < umi_project_task_registry_count(
             umi_project_workspace_task((UmiProjectWorkspace *)workspace)); ++index) {
        if (umi_project_task_registry_at(
                umi_project_workspace_task((UmiProjectWorkspace *)workspace),
                index, &task) != UMI_STATUS_OK ||
            !scoped_project_id_matches(task.project_id, project_id)) continue;
        if (task.enabled != 0 && task.command[0] == '\0') {
            scoped_add_issue(out_report, UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                             "task.empty-command", project_id, task.id,
                             "Enabled task does not contain a command.");
        }
        if (task.enabled != 0 && task.default_task != 0) default_tasks += 1U;
    }
    if (default_tasks > 1U) {
        scoped_add_issue(out_report, UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                         "task.multiple-defaults", project_id, "",
                         "Project contains more than one enabled default task.");
    }

    for (index = 0U; index < umi_project_launch_profile_registry_count(
             umi_project_workspace_launch_profile((UmiProjectWorkspace *)workspace));
         ++index) {
        if (umi_project_launch_profile_registry_at(
                umi_project_workspace_launch_profile((UmiProjectWorkspace *)workspace),
                index, &launch) != UMI_STATUS_OK ||
            !scoped_project_id_matches(launch.project_id, project_id)) continue;
        if (launch.program[0] == '\0') {
            scoped_add_issue(out_report, UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                             "launch.empty-program", project_id, launch.id,
                             "Launch profile does not contain a program.");
        }
        if (launch.default_profile != 0) default_launch_profiles += 1U;
    }
    if (default_launch_profiles > 1U) {
        scoped_add_issue(out_report, UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                         "launch.multiple-defaults", project_id, "",
                         "Project contains more than one default launch profile.");
    }

    for (index = 0U; index < umi_project_dependency_registry_count(
             umi_project_workspace_dependency((UmiProjectWorkspace *)workspace));
         ++index) {
        if (umi_project_dependency_registry_at(
                umi_project_workspace_dependency((UmiProjectWorkspace *)workspace),
                index, &dependency) != UMI_STATUS_OK ||
            !scoped_project_id_matches(dependency.project_id, project_id)) continue;
        if (dependency.optional == 0 && dependency.resolved == 0) {
            out_report->unresolved_required_dependency_count += 1U;
            scoped_add_issue(out_report, UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                             "dependency.unresolved", project_id, dependency.id,
                             "Required project dependency is unresolved.");
        }
    }

    for (index = 0U; index < umi_project_reference_registry_count(
             umi_project_workspace_reference((UmiProjectWorkspace *)workspace));
         ++index) {
        if (umi_project_reference_registry_at(
                umi_project_workspace_reference((UmiProjectWorkspace *)workspace),
                index, &reference) != UMI_STATUS_OK ||
            !scoped_project_id_matches(reference.project_id, project_id)) continue;
        status = reference.target_project_id[0] != '\0'
                     ? umi_project_descriptor_registry_find(
                           umi_project_workspace_descriptor((UmiProjectWorkspace *)workspace),
                           reference.target_project_id, &referenced_project)
                     : UMI_STATUS_NOT_FOUND;
        if (status != UMI_STATUS_OK && reference.required != 0) {
            scoped_add_issue(out_report, UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                             "reference.unresolved", project_id, reference.id,
                             "Required project reference cannot be resolved.");
        } else if (status != UMI_STATUS_OK) {
            scoped_add_issue(out_report, UMI_PROJECT_WORKSPACE_ISSUE_WARNING,
                             "reference.unavailable", project_id, reference.id,
                             "Optional project reference is unavailable.");
        }
    }

    for (index = 0U; index < umi_project_build_node_registry_count(
             umi_project_workspace_build_node((UmiProjectWorkspace *)workspace));
         ++index) {
        if (umi_project_build_node_registry_at(
                umi_project_workspace_build_node((UmiProjectWorkspace *)workspace),
                index, &node) != UMI_STATUS_OK ||
            !scoped_project_id_matches(node.project_id, project_id)) continue;
        if (node.target_id[0] == '\0') continue;
        status = umi_project_target_registry_find(
            umi_project_workspace_target((UmiProjectWorkspace *)workspace),
            node.target_id, &referenced_target);
        if (status != UMI_STATUS_OK ||
            !scoped_project_id_matches(referenced_target.project_id, project_id)) {
            scoped_add_issue(out_report, UMI_PROJECT_WORKSPACE_ISSUE_ERROR,
                             "build-node.target-unresolved", project_id, node.id,
                             "Build graph node refers to a missing or foreign target.");
        }
    }

    out_report->valid = out_report->error_count == 0U;
    return UMI_STATUS_OK;
}
