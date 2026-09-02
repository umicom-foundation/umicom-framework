/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/automation.h
 *
 * PURPOSE:
 *   Discover which parts of an Umicom workspace are affected by changed files
 *   and produce the smallest safe configure, build, test and deployment plan.
 *
 * ARCHITECTURE:
 *   This planner does not replace a compiler, CMake, Ninja or CTest. It is the
 *   decision layer above those tools. Source-control adapters, file watchers,
 *   Studio and command-line clients add changed paths; the Framework then maps
 *   those paths to registered product scopes and returns an executable plan.
 *
 * OWNERSHIP:
 *   The coordinator copies every scope and changed path. Plan items and the
 *   snapshot are returned as values, so callers never receive dangling
 *   pointers into Framework-owned storage. External synchronisation is needed
 *   when one coordinator is shared between threads.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_AUTOMATION_H
#define UMICOM_BUILD_AUTOMATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/build/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_AUTOMATION_API_VERSION 1U
#define UMI_BUILD_AUTOMATION_MAX_SCOPES 128U
#define UMI_BUILD_AUTOMATION_MAX_CHANGES 4096U
#define UMI_BUILD_AUTOMATION_REASON_CAPACITY 256U

/*
 * Each action is a bit so one plan item can say, for example, that a CMake
 * definition change needs configuration, compilation and focused tests.
 */
typedef uint32_t UmiBuildAutomationActionFlags;
enum {
    UMI_BUILD_AUTOMATION_ACTION_NONE = 0U,
    UMI_BUILD_AUTOMATION_ACTION_CONFIGURE = 1U << 0,
    UMI_BUILD_AUTOMATION_ACTION_BUILD = 1U << 1,
    UMI_BUILD_AUTOMATION_ACTION_TEST = 1U << 2,
    UMI_BUILD_AUTOMATION_ACTION_STAGE = 1U << 3,
    UMI_BUILD_AUTOMATION_ACTION_DEPLOY = 1U << 4,
    UMI_BUILD_AUTOMATION_ACTION_DOCUMENT = 1U << 5,
    UMI_BUILD_AUTOMATION_ACTION_ALL = (1U << 6) - 1U
};

/*
 * The kind explains why a file has an impact. The classifier understands the
 * common C, assembly, build, test, resource, manifest and documentation names;
 * callers may also supply a kind explicitly when they have richer knowledge.
 */
typedef enum UmiBuildAutomationChangeKind {
    UMI_BUILD_AUTOMATION_CHANGE_UNKNOWN = 0,
    UMI_BUILD_AUTOMATION_CHANGE_SOURCE = 1,
    UMI_BUILD_AUTOMATION_CHANGE_PUBLIC_HEADER = 2,
    UMI_BUILD_AUTOMATION_CHANGE_PRIVATE_HEADER = 3,
    UMI_BUILD_AUTOMATION_CHANGE_ASSEMBLY = 4,
    UMI_BUILD_AUTOMATION_CHANGE_BUILD_DEFINITION = 5,
    UMI_BUILD_AUTOMATION_CHANGE_TEST = 6,
    UMI_BUILD_AUTOMATION_CHANGE_RESOURCE = 7,
    UMI_BUILD_AUTOMATION_CHANGE_DOCUMENTATION = 8,
    UMI_BUILD_AUTOMATION_CHANGE_MANIFEST = 9,
    UMI_BUILD_AUTOMATION_CHANGE_GENERATED_INPUT = 10
} UmiBuildAutomationChangeKind;

/*
 * Scope flags describe relationships rather than tool commands. A shared
 * provider invalidates registered consumers when its compilable inputs change.
 * A workspace-wide scope invalidates every registered scope after a root build
 * definition changes.
 */
typedef uint32_t UmiBuildAutomationScopeFlags;
enum {
    UMI_BUILD_AUTOMATION_SCOPE_NONE = 0U,
    UMI_BUILD_AUTOMATION_SCOPE_SHARED_PROVIDER = 1U << 0,
    UMI_BUILD_AUTOMATION_SCOPE_CONSUMES_SHARED = 1U << 1,
    UMI_BUILD_AUTOMATION_SCOPE_WORKSPACE_WIDE = 1U << 2
};

/*
 * A scope connects one source path prefix with stable build-system names. The
 * source prefix uses forward slashes and may be empty for the workspace root.
 */
typedef struct UmiBuildAutomationScope {
    uint32_t structure_size;
    uint32_t api_version;
    char scope_id[UMI_BUILD_ID_CAPACITY];
    char display_name[UMI_BUILD_NAME_CAPACITY];
    char version[UMI_BUILD_NAME_CAPACITY];
    char source_prefix[UMI_BUILD_PATH_CAPACITY];
    char build_target[UMI_BUILD_NAME_CAPACITY];
    char test_build_target[UMI_BUILD_NAME_CAPACITY];
    char test_expression[UMI_BUILD_ARGUMENT_CAPACITY];
    char install_component[UMI_BUILD_NAME_CAPACITY];
    UmiBuildAutomationActionFlags default_actions;
    UmiBuildAutomationScopeFlags flags;
    int enabled;
} UmiBuildAutomationScope;

/* A change is a normalised workspace-relative path and its observed state. */
typedef struct UmiBuildAutomationChange {
    uint32_t structure_size;
    uint32_t api_version;
    char path[UMI_BUILD_PATH_CAPACITY];
    UmiBuildAutomationChangeKind kind;
    int deleted;
} UmiBuildAutomationChange;

/*
 * One plan item merges every changed file that affects the same product scope.
 * direct_change is true for a path inside this scope; inherited_change is true
 * when the scope is selected because it consumes a changed shared contract.
 */
typedef struct UmiBuildAutomationPlanItem {
    uint32_t structure_size;
    uint32_t api_version;
    char scope_id[UMI_BUILD_ID_CAPACITY];
    char display_name[UMI_BUILD_NAME_CAPACITY];
    char version[UMI_BUILD_NAME_CAPACITY];
    char build_target[UMI_BUILD_NAME_CAPACITY];
    char test_build_target[UMI_BUILD_NAME_CAPACITY];
    char test_expression[UMI_BUILD_ARGUMENT_CAPACITY];
    char install_component[UMI_BUILD_NAME_CAPACITY];
    char reason[UMI_BUILD_AUTOMATION_REASON_CAPACITY];
    UmiBuildAutomationActionFlags actions;
    size_t changed_file_count;
    size_t order;
    int direct_change;
    int inherited_change;
} UmiBuildAutomationPlanItem;

/* The snapshot lets dashboards report the plan without recounting its items. */
typedef struct UmiBuildAutomationSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    size_t scope_count;
    size_t change_count;
    size_t selected_count;
    size_t direct_count;
    size_t inherited_count;
    UmiBuildAutomationActionFlags actions;
    uint64_t revision;
    int evaluated;
} UmiBuildAutomationSnapshot;

typedef struct UmiBuildAutomation UmiBuildAutomation;

/* Initialise a scope before filling optional target and relationship fields. */
void umi_build_automation_scope_init(UmiBuildAutomationScope *scope,
                                     const char *scope_id,
                                     const char *display_name,
                                     const char *source_prefix);

/* Initialise and classify one workspace-relative changed path. */
void umi_build_automation_change_init(UmiBuildAutomationChange *change,
                                      const char *path,
                                      int deleted);

/* Classify a path without changing coordinator state. */
UmiBuildAutomationChangeKind umi_build_automation_classify_path(
    const char *path);

/* Allocate an empty coordinator whose bounded collections own their records. */
UmiStatus umi_build_automation_create(UmiBuildAutomation **out_automation);

/* Release all memory owned by a coordinator; passing NULL is safe. */
void umi_build_automation_destroy(UmiBuildAutomation *automation);

/* Remove scopes, changes and the evaluated plan while retaining the object. */
void umi_build_automation_clear(UmiBuildAutomation *automation);

/* Remove batch evidence while retaining the registered workspace scopes. */
void umi_build_automation_clear_changes(UmiBuildAutomation *automation);

/* Register or replace a scope with the same stable identifier. */
UmiStatus umi_build_automation_register_scope(
    UmiBuildAutomation *automation,
    const UmiBuildAutomationScope *scope);

/* Add or replace evidence for one changed path. */
UmiStatus umi_build_automation_add_change(
    UmiBuildAutomation *automation,
    const UmiBuildAutomationChange *change);

/* Build a deterministic direct and transitive plan from current evidence. */
UmiStatus umi_build_automation_evaluate(UmiBuildAutomation *automation);

/* Return a copied plan item in deterministic execution order. */
UmiStatus umi_build_automation_item_at(
    const UmiBuildAutomation *automation,
    size_t position,
    UmiBuildAutomationPlanItem *out_item);

/* Return the number of currently selected plan items. */
size_t umi_build_automation_count(const UmiBuildAutomation *automation);

/* Return aggregate counters and action flags for dashboards and commands. */
UmiStatus umi_build_automation_snapshot(
    const UmiBuildAutomation *automation,
    UmiBuildAutomationSnapshot *out_snapshot);

/* Format a set of action bits as a stable comma-separated description. */
const char *umi_build_automation_action_text(
    UmiBuildAutomationActionFlags actions,
    char *out_text,
    size_t capacity);

/* Return the stable display text for one classified change kind. */
const char *umi_build_automation_change_kind_text(
    UmiBuildAutomationChangeKind kind);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_BUILD_AUTOMATION_H */
