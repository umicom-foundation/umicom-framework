/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/lifecycle_plan.c
 *
 * PURPOSE:
 *   Build predictable verification, build, test, publication and repository
 *   update plans from reusable lifecycle stages.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/lifecycle_plan.h"

#include <string.h>

typedef struct UmiDeveloperLifecycleStageDefinition {
    UmiDeveloperLifecycleStage stage;
    const char *title;
    const char *explanation;
    int changes_local_state;
    int uses_network;
} UmiDeveloperLifecycleStageDefinition;

static const UmiDeveloperLifecycleStageDefinition lifecycle_stages[] = {
    {UMI_DEVELOPER_STAGE_ENVIRONMENT, "Environment",
     "Check compiler, CMake, Ninja, Git and their versions.", 0, 0},
    {UMI_DEVELOPER_STAGE_DEPENDENCIES, "Native dependencies",
     "Check required libraries and record the versions selected by pkg-config.", 0, 0},
    {UMI_DEVELOPER_STAGE_SOURCE_QUALITY, "Source quality",
     "Scan for likely defects, unsafe APIs and maintainability problems.", 0, 0},
    {UMI_DEVELOPER_STAGE_SOURCE_SECURITY, "Source security",
     "Scan source for security-sensitive patterns and missing validation.", 0, 0},
    {UMI_DEVELOPER_STAGE_ARCHITECTURE, "Architecture",
     "Check module boundaries, public contracts and duplicated ownership.", 0, 0},
    {UMI_DEVELOPER_STAGE_MEMORY_STATIC, "Memory safety",
     "Inspect allocation, buffer, ownership and resource-lifetime risks.", 0, 0},
    {UMI_DEVELOPER_STAGE_DEPENDENCY_VULNERABILITIES, "Dependency vulnerabilities",
     "Compare discovered dependency manifests and submodules with a CVE database.", 0, 1},
    {UMI_DEVELOPER_STAGE_CONFIGURE, "Configure",
     "Generate the selected CMake build tree from a named preset.", 1, 0},
    {UMI_DEVELOPER_STAGE_COMPILE_LINK, "Compile and link",
     "Compile source files and link libraries and applications through CMake.", 1, 0},
    {UMI_DEVELOPER_STAGE_TEST, "Tests",
     "Run the registered CTest suite and stop when a required test fails.", 1, 0},
    {UMI_DEVELOPER_STAGE_REPOSITORY_VERIFY, "Repository review",
     "Inspect repository, index and submodule state before a mutation.", 0, 0},
    {UMI_DEVELOPER_STAGE_REPOSITORY_STAGE, "Stage changes",
     "Stage additions, modifications and removals and show the resulting status.", 1, 0},
    {UMI_DEVELOPER_STAGE_REPOSITORY_COMMIT, "Commit",
     "Validate the staged diff and commit only when content exists.", 1, 0},
    {UMI_DEVELOPER_STAGE_REPOSITORY_PUSH, "Push",
     "Push the selected branch without force.", 0, 1},
    {UMI_DEVELOPER_STAGE_REPOSITORY_UPDATE, "Update",
     "Fetch and apply only a fast-forward update, then synchronise submodules.", 1, 1}
};

/* Provide the find stage operation used by this module and its client applications. */
static const UmiDeveloperLifecycleStageDefinition *find_stage(
    UmiDeveloperLifecycleStage stage)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < sizeof(lifecycle_stages) / sizeof(lifecycle_stages[0]);
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (lifecycle_stages[index].stage == stage) return &lifecycle_stages[index];
    }
    return NULL;
}

/* Provide the append stage operation used by this module and its client applications. */
static UmiStatus append_stage(UmiDeveloperLifecyclePlan *plan,
                              UmiDeveloperLifecycleStage stage,
                              int required)
{
    const UmiDeveloperLifecycleStageDefinition *definition = find_stage(stage);
    UmiDeveloperLifecyclePlanStage *item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || definition == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->stage_count >= UMI_DEVELOPER_LIFECYCLE_STAGE_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    item = &plan->stages[plan->stage_count++];
    item->stage = stage;
    item->required = required;
    item->changes_local_state = definition->changes_local_state;
    item->uses_network = definition->uses_network;
    item->title = definition->title;
    item->explanation = definition->explanation;
    return UMI_STATUS_OK;
}

/*
 * Provide the append verification operation used by this module and its client
 * applications.
 */
static UmiStatus append_verification(
    const UmiDeveloperLifecyclePlanRequest *request,
    UmiDeveloperLifecyclePlan *plan)
{
    static const UmiDeveloperLifecycleStage stages[] = {
        UMI_DEVELOPER_STAGE_ENVIRONMENT,
        UMI_DEVELOPER_STAGE_DEPENDENCIES,
        UMI_DEVELOPER_STAGE_SOURCE_QUALITY,
        UMI_DEVELOPER_STAGE_SOURCE_SECURITY,
        UMI_DEVELOPER_STAGE_ARCHITECTURE,
        UMI_DEVELOPER_STAGE_MEMORY_STATIC
    };
    size_t index;
    UmiStatus status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < sizeof(stages) / sizeof(stages[0]); ++index) {
        status = append_stage(plan, stages[index], 1);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return append_stage(
        plan,
        UMI_DEVELOPER_STAGE_DEPENDENCY_VULNERABILITIES,
        request->require_external_vulnerability_scanner != 0);
}

/*
 * Initialise developer lifecycle plan request from caller-provided values so later
 * operations receive a known state.
 */
void umi_developer_lifecycle_plan_request_init(
    UmiDeveloperLifecyclePlanRequest *request,
    UmiDeveloperLifecycleGoal goal)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL) return;
    (void)memset(request, 0, sizeof(*request));
    request->goal = goal;
}

/*
 * Provide the developer lifecycle plan build operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_lifecycle_plan_build(
    const UmiDeveloperLifecyclePlanRequest *request,
    UmiDeveloperLifecyclePlan *out_plan)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL || out_plan == NULL ||
        request->goal < UMI_DEVELOPER_LIFECYCLE_VERIFY ||
        request->goal > UMI_DEVELOPER_LIFECYCLE_COMPLETE)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_plan, 0, sizeof(*out_plan));
    out_plan->goal = request->goal;

    /* Apply this branch only when its contract condition is satisfied. */
    if (request->goal == UMI_DEVELOPER_LIFECYCLE_UPDATE) {
        status = append_stage(out_plan, UMI_DEVELOPER_STAGE_REPOSITORY_VERIFY, 1);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        return append_stage(out_plan, UMI_DEVELOPER_STAGE_REPOSITORY_UPDATE, 1);
    }

    status = append_verification(request, out_plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (request->goal == UMI_DEVELOPER_LIFECYCLE_VERIFY)
        return UMI_STATUS_OK;

    /* Apply this branch only when its contract condition is satisfied. */
    if (request->goal == UMI_DEVELOPER_LIFECYCLE_BUILD ||
        request->goal == UMI_DEVELOPER_LIFECYCLE_TEST ||
        request->goal == UMI_DEVELOPER_LIFECYCLE_COMPLETE) {
        status = append_stage(out_plan, UMI_DEVELOPER_STAGE_CONFIGURE, 1);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = append_stage(out_plan, UMI_DEVELOPER_STAGE_COMPILE_LINK, 1);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (request->goal == UMI_DEVELOPER_LIFECYCLE_TEST ||
        request->goal == UMI_DEVELOPER_LIFECYCLE_COMPLETE) {
        status = append_stage(out_plan, UMI_DEVELOPER_STAGE_TEST, 1);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (request->goal == UMI_DEVELOPER_LIFECYCLE_PUBLISH ||
        request->goal == UMI_DEVELOPER_LIFECYCLE_COMPLETE) {
        status = append_stage(out_plan, UMI_DEVELOPER_STAGE_REPOSITORY_VERIFY, 1);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = append_stage(out_plan, UMI_DEVELOPER_STAGE_REPOSITORY_STAGE, 1);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = append_stage(out_plan, UMI_DEVELOPER_STAGE_REPOSITORY_COMMIT, 1);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = append_stage(out_plan, UMI_DEVELOPER_STAGE_REPOSITORY_PUSH, 1);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the developer lifecycle goal text operation used by this module and its client
 * applications.
 */
const char *umi_developer_lifecycle_goal_text(UmiDeveloperLifecycleGoal goal)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (goal) {
        case UMI_DEVELOPER_LIFECYCLE_VERIFY: return "verify";
        case UMI_DEVELOPER_LIFECYCLE_BUILD: return "build";
        case UMI_DEVELOPER_LIFECYCLE_TEST: return "test";
        case UMI_DEVELOPER_LIFECYCLE_PUBLISH: return "publish";
        case UMI_DEVELOPER_LIFECYCLE_UPDATE: return "update";
        case UMI_DEVELOPER_LIFECYCLE_COMPLETE: return "complete";
        default: return "unknown";
    }
}

/*
 * Provide the developer lifecycle stage text operation used by this module and its client
 * applications.
 */
const char *umi_developer_lifecycle_stage_text(UmiDeveloperLifecycleStage stage)
{
    const UmiDeveloperLifecycleStageDefinition *definition = find_stage(stage);
    return definition != NULL ? definition->title : "Unknown stage";
}
