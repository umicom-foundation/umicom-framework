/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/task.c
 *
 * PURPOSE:
 *   Implement bounded workspace-task descriptors and registry ownership.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/build/task.h"

#include <stdlib.h>
#include <string.h>

struct UmiBuildTaskRegistry {
    UmiBuildTaskSnapshot tasks[UMI_BUILD_TASK_CAPACITY];
    size_t count;
    uint64_t revision;
};

static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

static void write_message(char *destination, size_t capacity,
                          const char *message)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (message == NULL) message = "";
    length = strlen(message);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, message, length);
    destination[length] = '\0';
}

static size_t find_index(const UmiBuildTaskRegistry *registry,
                         const char *task_id)
{
    size_t index;
    if (registry == NULL || task_id == NULL) return SIZE_MAX;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->tasks[index].task_id, task_id) == 0) return index;
    }
    return SIZE_MAX;
}

static int dependency_exists(const UmiBuildTaskSnapshot *task,
                             const char *task_id)
{
    size_t index;
    for (index = 0U; index < task->dependency_count; ++index) {
        if (strcmp(task->dependencies[index], task_id) == 0) return 1;
    }
    return 0;
}

void umi_build_task_init(UmiBuildTaskSnapshot *task,
                         const char *task_id,
                         const char *label,
                         UmiBuildTaskKind kind)
{
    if (task == NULL) return;
    (void)memset(task, 0, sizeof(*task));
    task->structure_size = (uint32_t)sizeof(*task);
    task->api_version = UMI_BUILD_TASK_API_VERSION;
    if (task_id != NULL)
        (void)copy_text(task->task_id, sizeof(task->task_id), task_id);
    if (label != NULL)
        (void)copy_text(task->label, sizeof(task->label), label);
    task->kind = kind;
    task->run_on = UMI_BUILD_TASK_RUN_MANUALLY;
    task->maximum_attempts = 1U;
    task->flags = UMI_BUILD_TASK_ENABLED;
    task->incremental = 1;
    task->revision = 1U;
    switch (kind) {
    case UMI_BUILD_TASK_CONFIGURE:
        task->build_phase = UMI_BUILD_PHASE_CONFIGURE;
        task->has_build_phase = 1;
        break;
    case UMI_BUILD_TASK_BUILD:
        task->build_phase = UMI_BUILD_PHASE_BUILD;
        task->has_build_phase = 1;
        break;
    case UMI_BUILD_TASK_TEST:
        task->build_phase = UMI_BUILD_PHASE_TEST;
        task->has_build_phase = 1;
        break;
    case UMI_BUILD_TASK_RUN:
        task->build_phase = UMI_BUILD_PHASE_RUN;
        task->has_build_phase = 1;
        break;
    case UMI_BUILD_TASK_CLEAN:
        task->build_phase = UMI_BUILD_PHASE_CLEAN;
        task->has_build_phase = 1;
        break;
    case UMI_BUILD_TASK_INSTALL:
    case UMI_BUILD_TASK_PACKAGE:
    case UMI_BUILD_TASK_DEPLOY:
        task->build_phase = UMI_BUILD_PHASE_INSTALL;
        task->has_build_phase = 1;
        break;
    case UMI_BUILD_TASK_COMMAND:
    case UMI_BUILD_TASK_COMPOSITE:
    default:
        break;
    }
}

UmiStatus umi_build_task_set_command(UmiBuildTaskSnapshot *task,
                                     const char *command,
                                     const char *working_directory)
{
    UmiStatus status;
    if (task == NULL || command == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = copy_text(task->command, sizeof(task->command), command);
    if (status == UMI_STATUS_OK && working_directory != NULL) {
        status = copy_text(task->working_directory,
                           sizeof(task->working_directory),
                           working_directory);
    }
    if (status == UMI_STATUS_OK) task->revision += 1U;
    return status;
}

UmiStatus umi_build_task_add_dependency(UmiBuildTaskSnapshot *task,
                                        const char *dependency_task_id)
{
    UmiStatus status;
    if (task == NULL || dependency_task_id == NULL ||
        dependency_task_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(task->task_id, dependency_task_id) == 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (dependency_exists(task, dependency_task_id)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (task->dependency_count >= UMI_BUILD_TASK_DEPENDENCY_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = copy_text(task->dependencies[task->dependency_count],
                       sizeof(task->dependencies[task->dependency_count]),
                       dependency_task_id);
    if (status == UMI_STATUS_OK) {
        task->dependency_count += 1U;
        task->revision += 1U;
    }
    return status;
}

static UmiStatus add_path(char entries[][UMI_BUILD_PATH_CAPACITY],
                          size_t *count, size_t limit, const char *value)
{
    size_t index;
    UmiStatus status;
    if (entries == NULL || count == NULL || value == NULL || value[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < *count; ++index) {
        if (strcmp(entries[index], value) == 0) return UMI_STATUS_ALREADY_EXISTS;
    }
    if (*count >= limit) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = copy_text(entries[*count], UMI_BUILD_PATH_CAPACITY, value);
    if (status == UMI_STATUS_OK) *count += 1U;
    return status;
}

UmiStatus umi_build_task_add_input(UmiBuildTaskSnapshot *task,
                                   const char *input_pattern)
{
    UmiStatus status;
    if (task == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = add_path(task->inputs, &task->input_count,
                      UMI_BUILD_TASK_INPUT_CAPACITY, input_pattern);
    if (status == UMI_STATUS_OK) task->revision += 1U;
    return status;
}

UmiStatus umi_build_task_add_output(UmiBuildTaskSnapshot *task,
                                    const char *output_pattern)
{
    UmiStatus status;
    if (task == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = add_path(task->outputs, &task->output_count,
                      UMI_BUILD_TASK_OUTPUT_CAPACITY, output_pattern);
    if (status == UMI_STATUS_OK) task->revision += 1U;
    return status;
}

UmiStatus umi_build_task_add_environment(UmiBuildTaskSnapshot *task,
                                         const char *name_value_pair)
{
    size_t index;
    UmiStatus status;
    if (task == NULL || name_value_pair == NULL ||
        strchr(name_value_pair, '=') == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < task->environment_count; ++index) {
        if (strcmp(task->environment[index], name_value_pair) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (task->environment_count >= UMI_BUILD_TASK_ENVIRONMENT_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = copy_text(task->environment[task->environment_count],
                       sizeof(task->environment[task->environment_count]),
                       name_value_pair);
    if (status == UMI_STATUS_OK) {
        task->environment_count += 1U;
        task->revision += 1U;
    }
    return status;
}

UmiStatus umi_build_task_validate(const UmiBuildTaskSnapshot *task,
                                  char *out_message,
                                  size_t message_capacity)
{
    size_t index;
    if (task == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (task->task_id[0] == '\0' || task->label[0] == '\0') {
        write_message(out_message, message_capacity,
                      "Task id and label are required.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (task->kind < UMI_BUILD_TASK_COMMAND ||
        task->kind > UMI_BUILD_TASK_COMPOSITE ||
        task->run_on < UMI_BUILD_TASK_RUN_MANUALLY ||
        task->run_on > UMI_BUILD_TASK_RUN_CONTINUOUSLY) {
        write_message(out_message, message_capacity,
                      "Task kind or run policy is invalid.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (task->kind == UMI_BUILD_TASK_COMPOSITE &&
        task->dependency_count == 0U) {
        write_message(out_message, message_capacity,
                      "Composite tasks require dependencies.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (task->kind != UMI_BUILD_TASK_COMPOSITE &&
        task->command[0] == '\0' && !task->has_build_phase) {
        write_message(out_message, message_capacity,
                      "Executable tasks require a command or build phase.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (task->has_build_phase &&
        (task->build_phase < UMI_BUILD_PHASE_CONFIGURE ||
         task->build_phase > UMI_BUILD_PHASE_INSTALL)) {
        write_message(out_message, message_capacity,
                      "Build phase is invalid.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (task->maximum_attempts == 0U ||
        (task->flags &
         ~(UmiBuildTaskFlags)UMI_BUILD_TASK_ALL_FLAGS) != 0U) {
        write_message(out_message, message_capacity,
                      "Retry or flag configuration is invalid.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < task->dependency_count; ++index) {
        if (task->dependencies[index][0] == '\0' ||
            strcmp(task->dependencies[index], task->task_id) == 0) {
            write_message(out_message, message_capacity,
                          "Dependencies must be non-empty and non-self.");
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    write_message(out_message, message_capacity, "Task is valid.");
    return UMI_STATUS_OK;
}

UmiStatus umi_build_task_registry_create(UmiBuildTaskRegistry **out_registry)
{
    UmiBuildTaskRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiBuildTaskRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_build_task_registry_destroy(UmiBuildTaskRegistry *registry)
{
    free(registry);
}

UmiStatus umi_build_task_registry_upsert(UmiBuildTaskRegistry *registry,
                                         const UmiBuildTaskSnapshot *task)
{
    UmiBuildTaskSnapshot copy;
    size_t index;
    char message[256];
    UmiStatus status;
    if (registry == NULL || task == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_task_validate(task, message, sizeof(message));
    if (status != UMI_STATUS_OK) return status;
    copy = *task;
    copy.structure_size = (uint32_t)sizeof(copy);
    copy.api_version = UMI_BUILD_TASK_API_VERSION;
    if (copy.revision == 0U) copy.revision = 1U;
    index = find_index(registry, task->task_id);
    if (index == SIZE_MAX) {
        if (registry->count >= UMI_BUILD_TASK_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        registry->tasks[registry->count++] = copy;
    } else {
        copy.revision = registry->tasks[index].revision + 1U;
        registry->tasks[index] = copy;
    }
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_task_registry_remove(UmiBuildTaskRegistry *registry,
                                         const char *task_id)
{
    size_t index;
    if (registry == NULL || task_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, task_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < registry->count) {
        (void)memmove(&registry->tasks[index], &registry->tasks[index + 1U],
                      (registry->count - index - 1U) * sizeof(registry->tasks[0]));
    }
    registry->count -= 1U;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_task_registry_find(const UmiBuildTaskRegistry *registry,
                                       const char *task_id,
                                       UmiBuildTaskSnapshot *out_task)
{
    size_t index;
    if (registry == NULL || task_id == NULL || out_task == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(registry, task_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_task = registry->tasks[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_build_task_registry_at(const UmiBuildTaskRegistry *registry,
                                     size_t index,
                                     UmiBuildTaskSnapshot *out_task)
{
    if (registry == NULL || out_task == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_task = registry->tasks[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_build_task_registry_snapshot(
    const UmiBuildTaskRegistry *registry,
    UmiBuildTaskRegistrySnapshot *out_snapshot)
{
    size_t index;
    if (registry == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_BUILD_TASK_API_VERSION;
    out_snapshot->task_count = registry->count;
    out_snapshot->revision = registry->revision;
    for (index = 0U; index < registry->count; ++index) {
        const UmiBuildTaskSnapshot *task = &registry->tasks[index];
        if ((task->flags & UMI_BUILD_TASK_ENABLED) != 0U)
            out_snapshot->enabled_count += 1U;
        if ((task->flags & UMI_BUILD_TASK_DEFAULT) != 0U)
            out_snapshot->default_count += 1U;
        if ((task->flags & UMI_BUILD_TASK_BACKGROUND) != 0U)
            out_snapshot->background_count += 1U;
        if (task->kind == UMI_BUILD_TASK_COMPOSITE)
            out_snapshot->composite_count += 1U;
    }
    return UMI_STATUS_OK;
}

size_t umi_build_task_registry_count(const UmiBuildTaskRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_build_task_registry_revision(const UmiBuildTaskRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}

void umi_build_task_registry_clear(UmiBuildTaskRegistry *registry)
{
    if (registry == NULL) return;
    (void)memset(registry->tasks, 0, sizeof(registry->tasks));
    registry->count = 0U;
    registry->revision += 1U;
}

const char *umi_build_task_kind_text(UmiBuildTaskKind kind)
{
    switch (kind) {
    case UMI_BUILD_TASK_COMMAND: return "Command";
    case UMI_BUILD_TASK_CONFIGURE: return "Configure";
    case UMI_BUILD_TASK_BUILD: return "Build";
    case UMI_BUILD_TASK_TEST: return "Test";
    case UMI_BUILD_TASK_RUN: return "Run";
    case UMI_BUILD_TASK_CLEAN: return "Clean";
    case UMI_BUILD_TASK_INSTALL: return "Install";
    case UMI_BUILD_TASK_PACKAGE: return "Package";
    case UMI_BUILD_TASK_DEPLOY: return "Deploy";
    case UMI_BUILD_TASK_COMPOSITE: return "Composite";
    default: return "Unknown";
    }
}

const char *umi_build_task_run_on_text(UmiBuildTaskRunOn run_on)
{
    switch (run_on) {
    case UMI_BUILD_TASK_RUN_MANUALLY: return "Manual";
    case UMI_BUILD_TASK_RUN_ON_WORKSPACE_OPEN: return "Workspace open";
    case UMI_BUILD_TASK_RUN_ON_FOLDER_OPEN: return "Folder open";
    case UMI_BUILD_TASK_RUN_ON_FILE_SAVE: return "File save";
    case UMI_BUILD_TASK_RUN_CONTINUOUSLY: return "Continuous";
    default: return "Unknown";
    }
}
