/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/task.h
 *
 * PURPOSE:
 *   Define provider-neutral workspace tasks that can represent native build
 *   phases, scripted commands, tests, packaging, deployment preparation and
 *   composite operations without exposing a shell, IDE toolkit or process API.
 *
 * ARCHITECTURE:
 *   Tasks are copied into bounded Framework-owned storage. Dependencies, inputs,
 *   outputs and environment overlays are declarative metadata; execution remains
 *   behind task-orchestration and supervised-process boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_TASK_H
#define UMICOM_BUILD_TASK_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/build/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_TASK_API_VERSION 1U
#define UMI_BUILD_TASK_CAPACITY 128U
#define UMI_BUILD_TASK_DEPENDENCY_CAPACITY 16U
#define UMI_BUILD_TASK_INPUT_CAPACITY 16U
#define UMI_BUILD_TASK_OUTPUT_CAPACITY 16U
#define UMI_BUILD_TASK_ENVIRONMENT_CAPACITY 16U
#define UMI_BUILD_TASK_ENVIRONMENT_ENTRY_CAPACITY 512U

typedef enum UmiBuildTaskKind {
    UMI_BUILD_TASK_COMMAND = 0,
    UMI_BUILD_TASK_CONFIGURE = 1,
    UMI_BUILD_TASK_BUILD = 2,
    UMI_BUILD_TASK_TEST = 3,
    UMI_BUILD_TASK_RUN = 4,
    UMI_BUILD_TASK_CLEAN = 5,
    UMI_BUILD_TASK_INSTALL = 6,
    UMI_BUILD_TASK_PACKAGE = 7,
    UMI_BUILD_TASK_DEPLOY = 8,
    UMI_BUILD_TASK_COMPOSITE = 9
} UmiBuildTaskKind;

typedef enum UmiBuildTaskRunOn {
    UMI_BUILD_TASK_RUN_MANUALLY = 0,
    UMI_BUILD_TASK_RUN_ON_WORKSPACE_OPEN = 1,
    UMI_BUILD_TASK_RUN_ON_FOLDER_OPEN = 2,
    UMI_BUILD_TASK_RUN_ON_FILE_SAVE = 3,
    UMI_BUILD_TASK_RUN_CONTINUOUSLY = 4
} UmiBuildTaskRunOn;

typedef uint32_t UmiBuildTaskFlags;
enum {
    UMI_BUILD_TASK_ENABLED = 1U << 0,
    UMI_BUILD_TASK_DEFAULT = 1U << 1,
    UMI_BUILD_TASK_HIDDEN = 1U << 2,
    UMI_BUILD_TASK_BACKGROUND = 1U << 3,
    UMI_BUILD_TASK_ALWAYS_RUN = 1U << 4,
    UMI_BUILD_TASK_REQUIRES_WORKSPACE_TRUST = 1U << 5,
    UMI_BUILD_TASK_ALLOW_PARALLEL = 1U << 6,
    UMI_BUILD_TASK_CONTINUE_ON_ERROR = 1U << 7,
    UMI_BUILD_TASK_ALL_FLAGS = (1U << 8) - 1U
};

typedef struct UmiBuildTaskSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    char task_id[UMI_BUILD_ID_CAPACITY];
    char label[UMI_BUILD_NAME_CAPACITY];
    char detail[UMI_BUILD_NAME_CAPACITY];
    char group[UMI_BUILD_NAME_CAPACITY];
    char category[UMI_BUILD_NAME_CAPACITY];
    char command[UMI_BUILD_COMMAND_CAPACITY];
    char working_directory[UMI_BUILD_PATH_CAPACITY];
    char problem_matcher[UMI_BUILD_NAME_CAPACITY];
    UmiBuildTaskKind kind;
    UmiBuildTaskRunOn run_on;
    UmiBuildPhase build_phase;
    int has_build_phase;
    char dependencies[UMI_BUILD_TASK_DEPENDENCY_CAPACITY]
                     [UMI_BUILD_ID_CAPACITY];
    size_t dependency_count;
    char inputs[UMI_BUILD_TASK_INPUT_CAPACITY][UMI_BUILD_PATH_CAPACITY];
    size_t input_count;
    char outputs[UMI_BUILD_TASK_OUTPUT_CAPACITY][UMI_BUILD_PATH_CAPACITY];
    size_t output_count;
    char environment[UMI_BUILD_TASK_ENVIRONMENT_CAPACITY]
                    [UMI_BUILD_TASK_ENVIRONMENT_ENTRY_CAPACITY];
    size_t environment_count;
    uint32_t timeout_ms;
    uint32_t maximum_attempts;
    UmiBuildTaskFlags flags;
    int incremental;
    uint64_t revision;
} UmiBuildTaskSnapshot;

typedef struct UmiBuildTaskRegistrySnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    size_t task_count;
    size_t enabled_count;
    size_t default_count;
    size_t background_count;
    size_t composite_count;
    uint64_t revision;
} UmiBuildTaskRegistrySnapshot;

typedef struct UmiBuildTaskRegistry UmiBuildTaskRegistry;

void umi_build_task_init(UmiBuildTaskSnapshot *task,
                         const char *task_id,
                         const char *label,
                         UmiBuildTaskKind kind);
UmiStatus umi_build_task_set_command(UmiBuildTaskSnapshot *task,
                                     const char *command,
                                     const char *working_directory);
UmiStatus umi_build_task_add_dependency(UmiBuildTaskSnapshot *task,
                                        const char *dependency_task_id);
UmiStatus umi_build_task_add_input(UmiBuildTaskSnapshot *task,
                                   const char *input_pattern);
UmiStatus umi_build_task_add_output(UmiBuildTaskSnapshot *task,
                                    const char *output_pattern);
UmiStatus umi_build_task_add_environment(UmiBuildTaskSnapshot *task,
                                         const char *name_value_pair);
UmiStatus umi_build_task_validate(const UmiBuildTaskSnapshot *task,
                                  char *out_message,
                                  size_t message_capacity);
UmiStatus umi_build_task_registry_create(UmiBuildTaskRegistry **out_registry);
void umi_build_task_registry_destroy(UmiBuildTaskRegistry *registry);
UmiStatus umi_build_task_registry_upsert(UmiBuildTaskRegistry *registry,
                                         const UmiBuildTaskSnapshot *task);
UmiStatus umi_build_task_registry_remove(UmiBuildTaskRegistry *registry,
                                         const char *task_id);
UmiStatus umi_build_task_registry_find(const UmiBuildTaskRegistry *registry,
                                       const char *task_id,
                                       UmiBuildTaskSnapshot *out_task);
UmiStatus umi_build_task_registry_at(const UmiBuildTaskRegistry *registry,
                                     size_t index,
                                     UmiBuildTaskSnapshot *out_task);
UmiStatus umi_build_task_registry_snapshot(
    const UmiBuildTaskRegistry *registry,
    UmiBuildTaskRegistrySnapshot *out_snapshot);
size_t umi_build_task_registry_count(const UmiBuildTaskRegistry *registry);
uint64_t umi_build_task_registry_revision(const UmiBuildTaskRegistry *registry);
void umi_build_task_registry_clear(UmiBuildTaskRegistry *registry);
const char *umi_build_task_kind_text(UmiBuildTaskKind kind);
const char *umi_build_task_run_on_text(UmiBuildTaskRunOn run_on);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_BUILD_TASK_H */
