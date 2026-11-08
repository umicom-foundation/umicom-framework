/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build/test_task_admission.c
 *
 * PURPOSE:
 *   Verify bounded task descriptors and unchanged state when public mutations cannot be admitted.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/task.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                   \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
static int Is(const char *a, const char *b) { return strcmp(a, b) == 0; }
static void Seed(UmiBuildTaskSnapshot *task) {
  umi_build_task_init(task, "build", "Build", UMI_BUILD_TASK_BUILD);
  (void)umi_build_task_set_command(task, "original-command", "original-directory");
  (void)umi_build_task_add_dependency(task, "configure");
  (void)umi_build_task_add_input(task, "source.c");
  (void)umi_build_task_add_output(task, "build/object.o");
  (void)umi_build_task_add_environment(task, "MODE=debug");
}
static int Invalid(UmiBuildTaskSnapshot *task) {
  char message[256] = "stale-success";
  UmiBuildTaskRegistry *registry = NULL;
  UmiBuildTaskSnapshot good, found;
  Seed(&good);
  CHECK(umi_build_task_registry_create(&registry) == UMI_STATUS_OK);
  CHECK(umi_build_task_registry_upsert(registry, &good) == UMI_STATUS_OK);
  const uint64_t before = umi_build_task_registry_revision(registry);
  CHECK(umi_build_task_validate(task, message, sizeof message) == UMI_STATUS_INVALID_ARGUMENT);
  CHECK(message[0] != '\0' && strcmp(message, "stale-success") != 0 &&
        strcmp(message, "Task is valid.") != 0);
  CHECK(umi_build_task_registry_upsert(registry, task) == UMI_STATUS_INVALID_ARGUMENT);
  CHECK(umi_build_task_registry_count(registry) == 1U &&
        umi_build_task_registry_revision(registry) == before);
  CHECK(umi_build_task_registry_find(registry, "build", &found) == UMI_STATUS_OK);
  CHECK(strcmp(found.command, "original-command") == 0);
  umi_build_task_registry_destroy(registry);
  return 0;
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *mode = argv[1];
  UmiBuildTaskSnapshot task, before;
  Seed(&task);
  if (Is(mode, "valid"))
    return umi_build_task_validate(&task, NULL, 0U) == UMI_STATUS_OK ? 0 : 1;
  if (Is(mode, "metadata-size"))
    task.structure_size = 0U;
  else if (Is(mode, "metadata-version"))
    task.api_version += 1U;
  else if (Is(mode, "dependency-count"))
    task.dependency_count = SIZE_MAX;
  else if (Is(mode, "input-count"))
    task.input_count = SIZE_MAX;
  else if (Is(mode, "output-count"))
    task.output_count = SIZE_MAX;
  else if (Is(mode, "environment-count"))
    task.environment_count = SIZE_MAX;
  else if (Is(mode, "scalar-text")) {
    struct Field {
      char *bytes;
      size_t size;
    } fields[] = {{task.task_id, sizeof task.task_id},
                  {task.label, sizeof task.label},
                  {task.detail, sizeof task.detail},
                  {task.group, sizeof task.group},
                  {task.category, sizeof task.category},
                  {task.command, sizeof task.command},
                  {task.working_directory, sizeof task.working_directory},
                  {task.problem_matcher, sizeof task.problem_matcher}};
    for (size_t i = 0U; i < sizeof fields / sizeof fields[0]; ++i) {
      Seed(&task);
      memset(fields[i].bytes, 'x', fields[i].size);
      CHECK(Invalid(&task) == 0);
    }
    return 0;
  } else if (Is(mode, "dependency-text"))
    memset(task.dependencies[0], 'x', sizeof task.dependencies[0]);
  else if (Is(mode, "input-text"))
    memset(task.inputs[0], 'x', sizeof task.inputs[0]);
  else if (Is(mode, "output-text"))
    memset(task.outputs[0], 'x', sizeof task.outputs[0]);
  else if (Is(mode, "environment-text"))
    memset(task.environment[0], 'x', sizeof task.environment[0]);
  else if (Is(mode, "invalid-kind"))
    task.kind = (UmiBuildTaskKind)99;
  else if (Is(mode, "invalid-run-policy"))
    task.run_on = (UmiBuildTaskRunOn)99;
  else if (Is(mode, "invalid-phase"))
    task.build_phase = (UmiBuildPhase)99;
  else if (Is(mode, "invalid-flags"))
    task.flags = UINT32_MAX;
  else if (Is(mode, "invalid-has-phase"))
    task.has_build_phase = 2;
  else if (Is(mode, "invalid-incremental"))
    task.incremental = -1;
  else if (Is(mode, "zero-attempts"))
    task.maximum_attempts = 0U;
  else if (Is(mode, "self-dependency"))
    strcpy(task.dependencies[0], task.task_id);
  else if (Is(mode, "duplicate-dependency")) {
    strcpy(task.dependencies[1], task.dependencies[0]);
    task.dependency_count = 2U;
  } else if (Is(mode, "empty-input"))
    task.inputs[0][0] = '\0';
  else if (Is(mode, "duplicate-input")) {
    strcpy(task.inputs[1], task.inputs[0]);
    task.input_count = 2U;
  } else if (Is(mode, "empty-output"))
    task.outputs[0][0] = '\0';
  else if (Is(mode, "duplicate-output")) {
    strcpy(task.outputs[1], task.outputs[0]);
    task.output_count = 2U;
  } else if (Is(mode, "environment-no-equals"))
    strcpy(task.environment[0], "MODE");
  else if (Is(mode, "environment-empty-name"))
    strcpy(task.environment[0], "=debug");
  else if (Is(mode, "duplicate-environment")) {
    strcpy(task.environment[1], task.environment[0]);
    task.environment_count = 2U;
  } else if (Is(mode, "incomplete-builder")) {
    umi_build_task_init(&task, "command", "Command", UMI_BUILD_TASK_COMMAND);
    CHECK(umi_build_task_add_input(&task, "source.c") == UMI_STATUS_OK);
    CHECK(umi_build_task_set_command(&task, "tool", NULL) == UMI_STATUS_OK);
    CHECK(umi_build_task_validate(&task, NULL, 0U) == UMI_STATUS_OK);
    return 0;
  } else if (Is(mode, "all-mutators-malformed") || Is(mode, "mutator-revision-limit")) {
    if (Is(mode, "all-mutators-malformed"))
      task.dependency_count = SIZE_MAX;
    else
      task.revision = UINT64_MAX;
    before = task;
    const UmiStatus expected = Is(mode, "all-mutators-malformed") ? UMI_STATUS_INVALID_ARGUMENT
                                                                  : UMI_STATUS_CAPACITY_EXCEEDED;
    CHECK(umi_build_task_set_command(&task, "changed", "new") == expected);
    CHECK(umi_build_task_add_dependency(&task, "other") == expected);
    CHECK(umi_build_task_add_input(&task, "other.c") == expected);
    CHECK(umi_build_task_add_output(&task, "other.o") == expected);
    CHECK(umi_build_task_add_environment(&task, "OTHER=value") == expected);
    CHECK(memcmp(&task, &before, sizeof task) == 0);
    return 0;
  } else if (Is(mode, "command-too-long") || Is(mode, "directory-too-long")) {
    char command[UMI_BUILD_COMMAND_CAPACITY + 1U], directory[UMI_BUILD_PATH_CAPACITY + 1U];
    memset(command, 'c', sizeof command - 1U);
    command[sizeof command - 1U] = '\0';
    memset(directory, 'd', sizeof directory - 1U);
    directory[sizeof directory - 1U] = '\0';
    before = task;
    CHECK(umi_build_task_set_command(&task, Is(mode, "command-too-long") ? command : "changed",
                                     directory) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&task, &before, sizeof task) == 0);
    return 0;
  } else if (Is(mode, "command-alias")) {
    const uint64_t revision = task.revision;
    CHECK(umi_build_task_set_command(&task, task.working_directory, task.command) == UMI_STATUS_OK);
    CHECK(strcmp(task.command, "original-directory") == 0 &&
          strcmp(task.working_directory, "original-command") == 0 &&
          task.revision == revision + 1U);
    return 0;
  } else if (Is(mode, "directory-retained")) {
    CHECK(umi_build_task_set_command(&task, "changed", NULL) == UMI_STATUS_OK);
    CHECK(strcmp(task.working_directory, "original-directory") == 0);
    return 0;
  } else if (Is(mode, "maximum-entries")) {
    for (size_t i = 1U; i < UMI_BUILD_TASK_DEPENDENCY_CAPACITY; ++i) {
      char id[32];
      snprintf(id, sizeof id, "dependency-%zu", i);
      CHECK(umi_build_task_add_dependency(&task, id) == UMI_STATUS_OK);
    }
    for (size_t i = 1U; i < UMI_BUILD_TASK_INPUT_CAPACITY; ++i) {
      char value[32];
      snprintf(value, sizeof value, "input-%zu", i);
      CHECK(umi_build_task_add_input(&task, value) == UMI_STATUS_OK);
    }
    for (size_t i = 1U; i < UMI_BUILD_TASK_OUTPUT_CAPACITY; ++i) {
      char value[32];
      snprintf(value, sizeof value, "output-%zu", i);
      CHECK(umi_build_task_add_output(&task, value) == UMI_STATUS_OK);
    }
    for (size_t i = 1U; i < UMI_BUILD_TASK_ENVIRONMENT_CAPACITY; ++i) {
      char value[32];
      snprintf(value, sizeof value, "VALUE_%zu=x", i);
      CHECK(umi_build_task_add_environment(&task, value) == UMI_STATUS_OK);
    }
    CHECK(umi_build_task_validate(&task, NULL, 0U) == UMI_STATUS_OK);
    before = task;
    CHECK(umi_build_task_add_dependency(&task, "extra") == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_build_task_add_input(&task, "extra") == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_build_task_add_output(&task, "extra") == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_build_task_add_environment(&task, "EXTRA=x") == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&task, &before, sizeof task) == 0);
    return 0;
  } else if (Is(mode, "unused-entry-ignored")) {
    memset(task.inputs[UMI_BUILD_TASK_INPUT_CAPACITY - 1U], 'x', UMI_BUILD_PATH_CAPACITY);
    CHECK(umi_build_task_validate(&task, NULL, 0U) == UMI_STATUS_OK);
    return 0;
  } else
    return 2;
  return Invalid(&task);
}
