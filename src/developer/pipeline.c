/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/pipeline.c
 *
 * PURPOSE:
 *   Implement deterministic developer-operation scheduling and lifecycle rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/pipeline.h"

#include <stdlib.h>
#include <string.h>

static UmiStatus operation_storage(const UmiDeveloperOperationSnapshot *operation, int require_metadata);

typedef struct UmiDeveloperDependency {
    size_t operation_index;
    size_t depends_on_index;
} UmiDeveloperDependency;

struct UmiDeveloperPipeline {
    UmiDeveloperOperationSnapshot *operations;
    UmiDeveloperDependency *dependencies;
    size_t operation_count;
    size_t dependency_count;
    uint64_t next_sequence;
    uint64_t revision;
};

/*
 * Provide the terminate operation operation used by this module and its client
 * applications.
 */
static void terminate_operation(UmiDeveloperOperationSnapshot *operation)
{
    size_t index;

    operation->id[UMI_DEVELOPER_ID_CAPACITY - 1U] = '\0';
    operation->title[UMI_DEVELOPER_TITLE_CAPACITY - 1U] = '\0';
    operation->project_id[UMI_DEVELOPER_ID_CAPACITY - 1U] = '\0';
    operation->configuration_id[UMI_DEVELOPER_ID_CAPACITY - 1U] = '\0';
    operation->target_id[UMI_DEVELOPER_ID_CAPACITY - 1U] = '\0';
    operation->program[UMI_DEVELOPER_PATH_CAPACITY - 1U] = '\0';
    operation->working_directory[UMI_DEVELOPER_PATH_CAPACITY - 1U] = '\0';
    operation->summary[UMI_DEVELOPER_SUMMARY_CAPACITY - 1U] = '\0';

    /* Apply this branch only when its contract condition is satisfied. */
    if (operation->argument_count > UMI_DEVELOPER_MAX_ARGUMENTS) {
        operation->argument_count = UMI_DEVELOPER_MAX_ARGUMENTS;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < operation->argument_count; ++index) {
        operation->arguments[index][UMI_DEVELOPER_ARGUMENT_CAPACITY - 1U] = '\0';
    }

    operation->struct_size = (uint32_t)sizeof(*operation);
    operation->api_version = UMI_DEVELOPER_OPERATION_API_VERSION;
    /* Apply this branch only when its contract condition is satisfied. */
    if (operation->max_attempts == 0U) {
        operation->max_attempts = 1U;
    }
}

/* Provide the copy summary operation used by this module and its client applications. */
static UmiStatus copy_summary(
    UmiDeveloperOperationSnapshot *operation,
    const char *summary)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (operation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (summary == NULL) {
        operation->summary[0] = '\0';
        return UMI_STATUS_OK;
    }

    length = strlen(summary);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= sizeof(operation->summary)) {
        length = sizeof(operation->summary) - 1U;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) {
        memcpy(operation->summary, summary, length);
    }
    operation->summary[length] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the find index operation used by this module and its client applications. */
static int find_index(
    const UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    size_t *out_index)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || operation_id == NULL || operation_id[0] == '\0') {
        return 0;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < pipeline->operation_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(pipeline->operations[index].id, operation_id) == 0) {
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (out_index != NULL) {
                *out_index = index;
            }
            return 1;
        }
    }
    return 0;
}

/* Provide the dependency exists operation used by this module and its client applications. */
static int dependency_exists(
    const UmiDeveloperPipeline *pipeline,
    size_t operation_index,
    size_t depends_on_index)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < pipeline->dependency_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (pipeline->dependencies[index].operation_index == operation_index &&
            pipeline->dependencies[index].depends_on_index == depends_on_index) {
            return 1;
        }
    }
    return 0;
}

/*
 * Provide the transitively depends on operation used by this module and its client
 * applications.
 */
/* Retain the former pop-time visitation for review. A dense acyclic graph
 * could enqueue the same pending node repeatedly and report stack exhaustion
 * as a cycle. The replacement marks nodes when queued, bounding storage by
 * distinct operations rather than by dependency paths. */
#if 0
static int transitively_depends_on(
    const UmiDeveloperPipeline *pipeline,
    size_t start_index,
    size_t target_index)
{
    size_t stack[UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY];
    unsigned char visited[UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY];
    size_t stack_count = 0U;

    memset(visited, 0, sizeof(visited));
    stack[stack_count++] = start_index;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (stack_count > 0U) {
        size_t current = stack[--stack_count];
        size_t dependency_index;

        /* Apply this branch only when its contract condition is satisfied. */
        if (current == target_index) {
            return 1;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (visited[current] != 0U) {
            continue;
        }
        visited[current] = 1U;

        /* Visit each bounded item once so every record receives the same rule. */
        for (dependency_index = 0U;
             dependency_index < pipeline->dependency_count;
             ++dependency_index) {
            const UmiDeveloperDependency *dependency =
                &pipeline->dependencies[dependency_index];

            /* Apply this branch only when its contract condition is satisfied. */
            if (dependency->operation_index == current &&
                visited[dependency->depends_on_index] == 0U) {
                /* Keep the operation inside its valid bounds before reading, writing or adding data. */
                if (stack_count >= UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY) {
                    return 1;
                }
                stack[stack_count++] = dependency->depends_on_index;
            }
        }
    }

    return 0;
}

#endif

static int transitively_depends_on(
    const UmiDeveloperPipeline *pipeline,
    size_t start_index,
    size_t target_index)
{
    size_t stack[UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY];
    unsigned char queued[UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY] = {0};
    size_t count = 0U;
    queued[start_index] = 1U;
    stack[count++] = start_index;
    while (count != 0U) {
        size_t current = stack[--count];
        if (current == target_index) return 1;
        for (size_t i = 0U; i < pipeline->dependency_count; ++i) {
            const UmiDeveloperDependency *edge = &pipeline->dependencies[i];
            if (edge->operation_index != current || queued[edge->depends_on_index])
                continue;
            /* Each admitted index is queued at most once. Keep a defensive
             * bound so corrupt private state cannot become an unchecked write. */
            if (count >= UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY) return 1;
            queued[edge->depends_on_index] = 1U;
            stack[count++] = edge->depends_on_index;
        }
    }
    return 0;
}

/*
 * Provide the dependencies succeeded operation used by this module and its client
 * applications.
 */
static int dependencies_succeeded(
    const UmiDeveloperPipeline *pipeline,
    size_t operation_index)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < pipeline->dependency_count; ++index) {
        const UmiDeveloperDependency *dependency = &pipeline->dependencies[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (dependency->operation_index == operation_index &&
            pipeline->operations[dependency->depends_on_index].state !=
                UMI_DEVELOPER_OPERATION_SUCCEEDED) {
            return 0;
        }
    }
    return 1;
}

/*
 * Provide the dependency terminal failure operation used by this module and its client
 * applications.
 */
/* Retain the direct model lookup for review. Transition projections now read
 * dependency state from the candidate arrays so cascading refusal is atomic. */
#if 0
static int dependency_terminal_failure(
    const UmiDeveloperPipeline *pipeline,
    size_t operation_index)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < pipeline->dependency_count; ++index) {
        const UmiDeveloperDependency *dependency = &pipeline->dependencies[index];
        UmiDeveloperOperationState state;

        /* Apply this branch only when its contract condition is satisfied. */
        if (dependency->operation_index != operation_index) {
            continue;
        }

        state = pipeline->operations[dependency->depends_on_index].state;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (state == UMI_DEVELOPER_OPERATION_FAILED ||
            state == UMI_DEVELOPER_OPERATION_CANCELLED ||
            state == UMI_DEVELOPER_OPERATION_BLOCKED) {
            return 1;
        }
    }
    return 0;
}
#endif



/* Project only mutable state and revision identities. Command text and argument
 * arrays remain owned by the pipeline, so ordinary transitions need neither a
 * large operation copy nor allocation. A failed projection never reaches the
 * publishing step. All calls retain the existing single-owner contract. */
typedef struct UmiDeveloperTransitionPlan {
    UmiDeveloperOperationState states[UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY];
    uint64_t revisions[UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY];
    uint64_t pipeline_revision;
    int has_extra_dependency;
    UmiDeveloperDependency extra_dependency;
} UmiDeveloperTransitionPlan;

static UmiStatus transition_plan_begin(const UmiDeveloperPipeline *pipeline,
    UmiDeveloperTransitionPlan *plan)
{
    if (pipeline == NULL || plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (pipeline->operations == NULL || pipeline->dependencies == NULL ||
        pipeline->operation_count > UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY ||
        pipeline->dependency_count > UMI_DEVELOPER_PIPELINE_DEPENDENCY_CAPACITY)
        return UMI_STATUS_INVALID_STATE;
    memset(plan, 0, sizeof(*plan));
    plan->pipeline_revision = pipeline->revision;
    for (size_t i = 0U; i < pipeline->operation_count; ++i) {
        UmiDeveloperOperationState state = pipeline->operations[i].state;
        /* Enum signedness is implementation-defined; an unsigned range test
         * also rejects negative representations without a redundant < 0 test. */
#if 0
        if (state < UMI_DEVELOPER_OPERATION_QUEUED ||
            state > UMI_DEVELOPER_OPERATION_BLOCKED) return UMI_STATUS_INVALID_STATE;
#endif
        if ((unsigned)state > (unsigned)UMI_DEVELOPER_OPERATION_BLOCKED)
            return UMI_STATUS_INVALID_STATE;
        plan->states[i] = state;
        plan->revisions[i] = pipeline->operations[i].revision;
    }
    for (size_t i = 0U; i < pipeline->dependency_count; ++i)
        if (pipeline->dependencies[i].operation_index >= pipeline->operation_count ||
            pipeline->dependencies[i].depends_on_index >= pipeline->operation_count)
            return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

static int transition_dependencies(const UmiDeveloperPipeline *pipeline,
    const UmiDeveloperTransitionPlan *plan, size_t operation_index,
    int require_success)
{
    size_t count = pipeline->dependency_count +
        (plan->has_extra_dependency != 0 ? 1U : 0U);
    for (size_t i = 0U; i < count; ++i) {
        const UmiDeveloperDependency *edge = i < pipeline->dependency_count ?
            &pipeline->dependencies[i] : &plan->extra_dependency;
        if (edge->operation_index != operation_index) continue;
        UmiDeveloperOperationState state = plan->states[edge->depends_on_index];
        if (require_success != 0) {
            if (state != UMI_DEVELOPER_OPERATION_SUCCEEDED) return 0;
        } else if (state == UMI_DEVELOPER_OPERATION_FAILED ||
                   state == UMI_DEVELOPER_OPERATION_CANCELLED ||
                   state == UMI_DEVELOPER_OPERATION_BLOCKED) return 1;
    }
    return require_success != 0 ? 1 : 0;
}

static UmiStatus transition_plan_tick(UmiDeveloperTransitionPlan *plan,
    size_t index, UmiDeveloperOperationState state)
{
    if (plan->revisions[index] == UINT64_MAX ||
        plan->pipeline_revision == UINT64_MAX) return UMI_STATUS_INVALID_STATE;
    plan->states[index] = state;
    ++plan->revisions[index];
    ++plan->pipeline_revision;
    return UMI_STATUS_OK;
}

static UmiStatus transition_plan_refresh(const UmiDeveloperPipeline *pipeline,
    UmiDeveloperTransitionPlan *plan)
{
    int changed = 0;
    /* A DAG settles within its number of vertices. Preserve the former ordered
     * cascade and its per-operation revision increments, with a bounded final
     * no-change pass. Even a late exhausted descendant refuses the whole plan. */
    for (size_t pass = 0U; pass <= pipeline->operation_count; ++pass) {
        int pass_changed = 0;
        for (size_t i = 0U; i < pipeline->operation_count; ++i) {
            UmiDeveloperOperationState state = plan->states[i];
            if (state != UMI_DEVELOPER_OPERATION_QUEUED &&
                state != UMI_DEVELOPER_OPERATION_BLOCKED) continue;
            UmiDeveloperOperationState next = transition_dependencies(pipeline, plan, i, 0) ?
                UMI_DEVELOPER_OPERATION_BLOCKED : UMI_DEVELOPER_OPERATION_QUEUED;
            if (state == next) continue;
            if (plan->revisions[i] == UINT64_MAX) return UMI_STATUS_INVALID_STATE;
            plan->states[i] = next;
            ++plan->revisions[i];
            pass_changed = 1;
            changed = 1;
        }
        if (pass_changed == 0) {
            if (changed != 0) {
                if (plan->pipeline_revision == UINT64_MAX) return UMI_STATUS_INVALID_STATE;
                ++plan->pipeline_revision;
            }
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_INVALID_STATE;
}

static void transition_plan_publish(UmiDeveloperPipeline *pipeline,
    const UmiDeveloperTransitionPlan *plan)
{
    for (size_t i = 0U; i < pipeline->operation_count; ++i) {
        pipeline->operations[i].state = plan->states[i];
        pipeline->operations[i].revision = plan->revisions[i];
    }
    pipeline->revision = plan->pipeline_revision;
}

static UmiStatus transition_plan_start(const UmiDeveloperPipeline *pipeline,
    UmiDeveloperTransitionPlan *plan, size_t index)
{
    const UmiDeveloperOperationSnapshot *operation = &pipeline->operations[index];
    if (plan->states[index] != UMI_DEVELOPER_OPERATION_QUEUED ||
        !transition_dependencies(pipeline, plan, index, 1) ||
        operation->attempt_count >= operation->max_attempts)
        return UMI_STATUS_INVALID_STATE;
    return transition_plan_tick(plan, index, UMI_DEVELOPER_OPERATION_RUNNING);
}

static UmiStatus transition_plan_next(const UmiDeveloperPipeline *pipeline,
    const UmiDeveloperTransitionPlan *plan, size_t *out_index)
{
    uint64_t sequence = UINT64_MAX;
    int found = 0;
    for (size_t i = 0U; i < pipeline->operation_count; ++i) {
        if (plan->states[i] == UMI_DEVELOPER_OPERATION_QUEUED &&
            transition_dependencies(pipeline, plan, i, 1) &&
            pipeline->operations[i].sequence < sequence) {
            sequence = pipeline->operations[i].sequence;
            *out_index = i;
            found = 1;
        }
    }
    return found != 0 ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

static void transition_publish_start(UmiDeveloperPipeline *pipeline,
    const UmiDeveloperTransitionPlan *plan, size_t index)
{
    transition_plan_publish(pipeline, plan);
    ++pipeline->operations[index].attempt_count;
    pipeline->operations[index].progress_basis_points = 0U;
    pipeline->operations[index].has_exit_code = 0;
}

/*
 * Initialise developer pipeline from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_developer_pipeline_create(UmiDeveloperPipeline **out_pipeline)
{
    UmiDeveloperPipeline *pipeline;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_pipeline == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_pipeline = NULL;
    pipeline = (UmiDeveloperPipeline *)calloc(1U, sizeof(*pipeline));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    pipeline->operations = (UmiDeveloperOperationSnapshot *)calloc(
        UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY,
        sizeof(*pipeline->operations));
    pipeline->dependencies = (UmiDeveloperDependency *)calloc(
        UMI_DEVELOPER_PIPELINE_DEPENDENCY_CAPACITY,
        sizeof(*pipeline->dependencies));

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline->operations == NULL || pipeline->dependencies == NULL) {
        umi_developer_pipeline_destroy(pipeline);
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    pipeline->next_sequence = 1U;
    pipeline->revision = 1U;
    *out_pipeline = pipeline;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by developer pipeline so the same storage can be reused
 * safely.
 */
void umi_developer_pipeline_destroy(UmiDeveloperPipeline *pipeline)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL) {
        return;
    }
    free(pipeline->dependencies);
    free(pipeline->operations);
    free(pipeline);
}

/*
 * Provide the developer pipeline submit operation used by this module and its client
 * applications.
 */
/* The previous single-operation path searched an unadmitted ID and then
 * silently terminated copied fields. Retain it for review. Shared fixed-storage
 * admission now runs before lookup, preserving valid legacy metadata defaults
 * while refusing unreadable IDs and truncated command values. */
#if 0
UmiStatus umi_developer_pipeline_submit(
    UmiDeveloperPipeline *pipeline,
    const UmiDeveloperOperationSnapshot *operation)
{
    UmiDeveloperOperationSnapshot copy;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || operation == NULL || operation->id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (pipeline->operation_count >= UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (find_index(pipeline, operation->id, NULL)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (operation->argument_count > UMI_DEVELOPER_MAX_ARGUMENTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    copy = *operation;
    terminate_operation(&copy);
    copy.state = UMI_DEVELOPER_OPERATION_QUEUED;
    copy.attempt_count = 0U;
    copy.exit_code = 0;
    copy.has_exit_code = 0;
    copy.progress_basis_points = 0U;
    copy.sequence = pipeline->next_sequence++;
    copy.revision = 1U;

    pipeline->operations[pipeline->operation_count++] = copy;
    pipeline->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_developer_pipeline_submit(
    UmiDeveloperPipeline *pipeline,
    const UmiDeveloperOperationSnapshot *operation)
{
    UmiDeveloperOperationSnapshot copy;
    UmiStatus validation;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || operation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    validation = operation_storage(operation, 0);
    if (validation != UMI_STATUS_OK) return validation;
    if (pipeline->next_sequence == UINT64_MAX || pipeline->revision == UINT64_MAX)
        return UMI_STATUS_INVALID_STATE;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (pipeline->operation_count >= UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (find_index(pipeline, operation->id, NULL)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (operation->argument_count > UMI_DEVELOPER_MAX_ARGUMENTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    copy = *operation;
    terminate_operation(&copy);
    copy.state = UMI_DEVELOPER_OPERATION_QUEUED;
    copy.attempt_count = 0U;
    copy.exit_code = 0;
    copy.has_exit_code = 0;
    copy.progress_basis_points = 0U;
    copy.sequence = pipeline->next_sequence++;
    copy.revision = 1U;

    pipeline->operations[pipeline->operation_count++] = copy;
    pipeline->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the developer pipeline add dependency operation used by this module and its
 * client applications.
 */
/* Retain edge insertion before refresh for review. The candidate edge participates in projected dependency resolution; exhausted descendants leave both the graph and state unchanged. */
#if 0
UmiStatus umi_developer_pipeline_add_dependency(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    const char *depends_on_operation_id)
{
    size_t operation_index;
    size_t depends_on_index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || operation_id == NULL ||
        depends_on_operation_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!find_index(pipeline, operation_id, &operation_index) ||
        !find_index(pipeline, depends_on_operation_id, &depends_on_index)) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (operation_index == depends_on_index) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (dependency_exists(pipeline, operation_index, depends_on_index)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (pipeline->dependency_count >=
        UMI_DEVELOPER_PIPELINE_DEPENDENCY_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /*
     * operation -> dependency is a backwards edge.  Adding it is cyclic when
     * the proposed dependency already (directly or indirectly) depends on the
     * operation.
     */
    if (transitively_depends_on(pipeline, depends_on_index, operation_index)) {
        return UMI_STATUS_INVALID_STATE;
    }

    pipeline->dependencies[pipeline->dependency_count].operation_index =
        operation_index;
    pipeline->dependencies[pipeline->dependency_count].depends_on_index =
        depends_on_index;
    pipeline->dependency_count += 1U;
    pipeline->revision += 1U;
    return umi_developer_pipeline_refresh(pipeline);
}
#endif

UmiStatus umi_developer_pipeline_add_dependency(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    const char *depends_on_operation_id)
{
    size_t operation_index;
    size_t depends_on_index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || operation_id == NULL ||
        depends_on_operation_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!find_index(pipeline, operation_id, &operation_index) ||
        !find_index(pipeline, depends_on_operation_id, &depends_on_index)) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (operation_index == depends_on_index) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (dependency_exists(pipeline, operation_index, depends_on_index)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (pipeline->dependency_count >=
        UMI_DEVELOPER_PIPELINE_DEPENDENCY_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /*
     * operation -> dependency is a backwards edge.  Adding it is cyclic when
     * the proposed dependency already (directly or indirectly) depends on the
     * operation.
     */
    if (transitively_depends_on(pipeline, depends_on_index, operation_index)) {
        return UMI_STATUS_INVALID_STATE;
    }

    UmiDeveloperTransitionPlan plan;
    UmiStatus status = transition_plan_begin(pipeline, &plan);
    if (status != UMI_STATUS_OK) return status;
    if (plan.pipeline_revision == UINT64_MAX) return UMI_STATUS_INVALID_STATE;
    ++plan.pipeline_revision;
    plan.has_extra_dependency = 1;
    plan.extra_dependency.operation_index = operation_index;
    plan.extra_dependency.depends_on_index = depends_on_index;
    status = transition_plan_refresh(pipeline, &plan);
    if (status != UMI_STATUS_OK) return status;
    pipeline->dependencies[pipeline->dependency_count++] = plan.extra_dependency;
    transition_plan_publish(pipeline, &plan);
    return UMI_STATUS_OK;
}


/* A chain is admitted as one candidate so a late duplicate or capacity refusal
 * cannot leave earlier operations runnable. Reuse the normal submit/dependency
 * rules on owned temporary storage; keep the public pipeline object address
 * stable for runtime and UI observers when the candidate is published. */
static UmiStatus operation_storage(const UmiDeveloperOperationSnapshot *operation, int require_metadata)
{
    if ((require_metadata && (operation->struct_size != sizeof(*operation) ||
        operation->api_version != UMI_DEVELOPER_OPERATION_API_VERSION)) ||
        operation->kind < UMI_DEVELOPER_OPERATION_CUSTOM ||
        operation->kind > UMI_DEVELOPER_OPERATION_AI)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (operation->argument_count > UMI_DEVELOPER_MAX_ARGUMENTS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
#define CHAIN_TERMINATED(field) \
    (memchr(operation->field, 0, sizeof(operation->field)) != NULL)
    if (!CHAIN_TERMINATED(id) || !CHAIN_TERMINATED(title) ||
        !CHAIN_TERMINATED(project_id) || !CHAIN_TERMINATED(configuration_id) ||
        !CHAIN_TERMINATED(target_id) || !CHAIN_TERMINATED(program) ||
        !CHAIN_TERMINATED(working_directory) || !CHAIN_TERMINATED(summary))
        return UMI_STATUS_CAPACITY_EXCEEDED;
#undef CHAIN_TERMINATED
    if (operation->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < operation->argument_count; ++i)
        if (memchr(operation->arguments[i], 0, sizeof(operation->arguments[i])) == NULL)
            return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}
UmiStatus umi_developer_pipeline_submit_chain(
    UmiDeveloperPipeline *pipeline,
    const UmiDeveloperOperationSnapshot *operations,
    size_t count)
{
    if (pipeline == NULL || operations == NULL || count == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY ||
        pipeline->operation_count > UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY ||
        pipeline->dependency_count > UMI_DEVELOPER_PIPELINE_DEPENDENCY_CAPACITY ||
        count > UMI_DEVELOPER_PIPELINE_OPERATION_CAPACITY - pipeline->operation_count ||
        count - 1U > UMI_DEVELOPER_PIPELINE_DEPENDENCY_CAPACITY - pipeline->dependency_count)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Reserve a conservative revision budget for submit, edge insertion and
     * refresh. Refusal preserves identities instead of allowing wraparound. */
    if ((uint64_t)count > UINT64_MAX - pipeline->next_sequence ||
        (uint64_t)(count * 3U) > UINT64_MAX - pipeline->revision)
        return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < pipeline->operation_count; ++i)
        if ((uint64_t)count > UINT64_MAX - pipeline->operations[i].revision)
            return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < count; ++i) {
        UmiStatus status = operation_storage(&operations[i], 1);
        if (status != UMI_STATUS_OK) return status;
    }
    UmiDeveloperPipeline *candidate = NULL;
    UmiStatus status = umi_developer_pipeline_create(&candidate);
    if (status != UMI_STATUS_OK) return status;
    memcpy(candidate->operations, pipeline->operations,
           pipeline->operation_count * sizeof(*pipeline->operations));
    memcpy(candidate->dependencies, pipeline->dependencies,
           pipeline->dependency_count * sizeof(*pipeline->dependencies));
    candidate->operation_count = pipeline->operation_count;
    candidate->dependency_count = pipeline->dependency_count;
    candidate->next_sequence = pipeline->next_sequence;
    candidate->revision = pipeline->revision;
    for (size_t i = 0U; i < count && status == UMI_STATUS_OK; ++i) {
        status = umi_developer_pipeline_submit(candidate, &operations[i]);
        if (status == UMI_STATUS_OK && i != 0U)
            status = umi_developer_pipeline_add_dependency(
                candidate, operations[i].id, operations[i - 1U].id);
    }
    if (status == UMI_STATUS_OK) {
        UmiDeveloperPipeline previous = *pipeline;
        *pipeline = *candidate;
        *candidate = previous;
    }
    umi_developer_pipeline_destroy(candidate);
    return status;
}

/*
 * Find developer pipeline while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_developer_pipeline_find(
    const UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    UmiDeveloperOperationSnapshot *out_operation)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || operation_id == NULL || out_operation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!find_index(pipeline, operation_id, &index)) {
        return UMI_STATUS_NOT_FOUND;
    }

    *out_operation = pipeline->operations[index];
    return UMI_STATUS_OK;
}

/*
 * Find developer pipeline while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_developer_pipeline_at(
    const UmiDeveloperPipeline *pipeline,
    size_t index,
    UmiDeveloperOperationSnapshot *out_operation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || out_operation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= pipeline->operation_count) {
        return UMI_STATUS_NOT_FOUND;
    }

    *out_operation = pipeline->operations[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by developer pipeline without changing their
 * state.
 */
size_t umi_developer_pipeline_count(const UmiDeveloperPipeline *pipeline)
{
    return pipeline != NULL ? pipeline->operation_count : 0U;
}

/*
 * Provide the developer pipeline revision operation used by this module and its client
 * applications.
 */
uint64_t umi_developer_pipeline_revision(const UmiDeveloperPipeline *pipeline)
{
    return pipeline != NULL ? pipeline->revision : 0U;
}

/*
 * Provide the developer pipeline refresh operation used by this module and its client
 * applications.
 */
/* Retain the immediate cascade for review. The projected replacement checks all affected identities before any authoritative state changes. */
#if 0
UmiStatus umi_developer_pipeline_refresh(UmiDeveloperPipeline *pipeline)
{
    size_t index;
    int changed = 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Repeat until stable because changing one blocked node can affect another
     * node later in a dependency chain.
     */
    for (;;) {
        int pass_changed = 0;

        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < pipeline->operation_count; ++index) {
            UmiDeveloperOperationSnapshot *operation =
                &pipeline->operations[index];

            /* Apply this branch only when its contract condition is satisfied. */
            if (operation->state == UMI_DEVELOPER_OPERATION_QUEUED &&
                dependency_terminal_failure(pipeline, index)) {
                operation->state = UMI_DEVELOPER_OPERATION_BLOCKED;
                operation->revision += 1U;
                pass_changed = 1;
            } else /* Apply this branch only when its contract condition is satisfied. */ if (operation->state == UMI_DEVELOPER_OPERATION_BLOCKED &&
                       !dependency_terminal_failure(pipeline, index)) {
                operation->state = UMI_DEVELOPER_OPERATION_QUEUED;
                operation->revision += 1U;
                pass_changed = 1;
            }
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (pass_changed == 0) {
            break;
        }
        changed = 1;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (changed != 0) {
        pipeline->revision += 1U;
    }
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_developer_pipeline_refresh(UmiDeveloperPipeline *pipeline)
{
    UmiDeveloperTransitionPlan plan;
    UmiStatus status = transition_plan_begin(pipeline, &plan);
    if (status == UMI_STATUS_OK) status = transition_plan_refresh(pipeline, &plan);
    if (status == UMI_STATUS_OK) transition_plan_publish(pipeline, &plan);
    return status;
}

/*
 * Provide the developer pipeline next ready operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_pipeline_next_ready(
    UmiDeveloperPipeline *pipeline,
    UmiDeveloperOperationSnapshot *out_operation)
{
    size_t index;
    size_t best_index = 0U;
    uint64_t best_sequence = UINT64_MAX;
    int found = 0;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || out_operation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_pipeline_refresh(pipeline);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < pipeline->operation_count; ++index) {
        const UmiDeveloperOperationSnapshot *operation =
            &pipeline->operations[index];

        /* Apply this branch only when its contract condition is satisfied. */
        if (operation->state == UMI_DEVELOPER_OPERATION_QUEUED &&
            dependencies_succeeded(pipeline, index) &&
            operation->sequence < best_sequence) {
            best_index = index;
            best_sequence = operation->sequence;
            found = 1;
        }
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (found == 0) {
        return UMI_STATUS_NOT_FOUND;
    }

    *out_operation = pipeline->operations[best_index];
    return UMI_STATUS_OK;
}

/*
 * Provide the developer pipeline start operation used by this module and its client
 * applications.
 */
/* Retain refresh-before-admission for review. Start now publishes dependency refresh and the selected start together, preserving the model on refusal. */
#if 0
UmiStatus umi_developer_pipeline_start(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id)
{
    size_t index;
    UmiDeveloperOperationSnapshot *operation;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || operation_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_pipeline_refresh(pipeline);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!find_index(pipeline, operation_id, &index)) {
        return UMI_STATUS_NOT_FOUND;
    }

    operation = &pipeline->operations[index];
    /* Apply this branch only when its contract condition is satisfied. */
    if (operation->state != UMI_DEVELOPER_OPERATION_QUEUED ||
        !dependencies_succeeded(pipeline, index)) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (operation->attempt_count >= operation->max_attempts) {
        return UMI_STATUS_INVALID_STATE;
    }

    operation->state = UMI_DEVELOPER_OPERATION_RUNNING;
    operation->attempt_count += 1U;
    operation->progress_basis_points = 0U;
    operation->has_exit_code = 0;
    operation->revision += 1U;
    pipeline->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_developer_pipeline_start(
    UmiDeveloperPipeline *pipeline, const char *operation_id)
{
    if (pipeline == NULL || operation_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index;
    if (!find_index(pipeline, operation_id, &index)) return UMI_STATUS_NOT_FOUND;
    UmiDeveloperTransitionPlan plan;
    UmiStatus status = transition_plan_begin(pipeline, &plan);
    if (status == UMI_STATUS_OK) status = transition_plan_refresh(pipeline, &plan);
    if (status == UMI_STATUS_OK) status = transition_plan_start(pipeline, &plan, index);
    if (status == UMI_STATUS_OK) transition_publish_start(pipeline, &plan, index);
    return status;
}

/*
 * Provide the developer pipeline set progress operation used by this module and its client
 * applications.
 */
/* Retain unchecked progress publication for review. Identity admission now precedes progress and summary mutation. */
#if 0
UmiStatus umi_developer_pipeline_set_progress(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    uint32_t progress_basis_points,
    const char *summary)
{
    size_t index;
    UmiDeveloperOperationSnapshot *operation;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || operation_id == NULL ||
        progress_basis_points > 10000U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!find_index(pipeline, operation_id, &index)) {
        return UMI_STATUS_NOT_FOUND;
    }

    operation = &pipeline->operations[index];
    /* Apply this branch only when its contract condition is satisfied. */
    if (operation->state != UMI_DEVELOPER_OPERATION_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }

    operation->progress_basis_points = progress_basis_points;
    (void)copy_summary(operation, summary);
    operation->revision += 1U;
    pipeline->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_developer_pipeline_set_progress(
    UmiDeveloperPipeline *pipeline, const char *operation_id,
    uint32_t progress_basis_points, const char *summary)
{
    if (pipeline == NULL || operation_id == NULL || progress_basis_points > 10000U)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index;
    if (!find_index(pipeline, operation_id, &index)) return UMI_STATUS_NOT_FOUND;
    if (pipeline->operations[index].state != UMI_DEVELOPER_OPERATION_RUNNING)
        return UMI_STATUS_INVALID_STATE;
    UmiDeveloperTransitionPlan plan;
    UmiStatus status = transition_plan_begin(pipeline, &plan);
    if (status == UMI_STATUS_OK)
        status = transition_plan_tick(&plan, index, UMI_DEVELOPER_OPERATION_RUNNING);
    if (status != UMI_STATUS_OK) return status;
    transition_plan_publish(pipeline, &plan);
    pipeline->operations[index].progress_basis_points = progress_basis_points;
    (void)copy_summary(&pipeline->operations[index], summary);
    return UMI_STATUS_OK;
}

/* Provide the finish operation operation used by this module and its client applications. */
/* Retain terminal mutation before refresh for review. The replacement projects every dependent state and revision before committing the result. */
#if 0
static UmiStatus finish_operation(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    UmiDeveloperOperationState state,
    int exit_code,
    const char *summary)
{
    size_t index;
    UmiDeveloperOperationSnapshot *operation;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || operation_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!find_index(pipeline, operation_id, &index)) {
        return UMI_STATUS_NOT_FOUND;
    }

    operation = &pipeline->operations[index];
    /* Apply this branch only when its contract condition is satisfied. */
    if (operation->state != UMI_DEVELOPER_OPERATION_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }

    operation->state = state;
    operation->exit_code = exit_code;
    operation->has_exit_code = 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (state == UMI_DEVELOPER_OPERATION_SUCCEEDED) {
        operation->progress_basis_points = 10000U;
    }
    (void)copy_summary(operation, summary);
    operation->revision += 1U;
    pipeline->revision += 1U;
    return umi_developer_pipeline_refresh(pipeline);
}
#endif

static UmiStatus finish_operation(
    UmiDeveloperPipeline *pipeline, const char *operation_id,
    UmiDeveloperOperationState state, int exit_code, const char *summary)
{
    if (pipeline == NULL || operation_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index;
    if (!find_index(pipeline, operation_id, &index)) return UMI_STATUS_NOT_FOUND;
    if (pipeline->operations[index].state != UMI_DEVELOPER_OPERATION_RUNNING)
        return UMI_STATUS_INVALID_STATE;
    UmiDeveloperTransitionPlan plan;
    UmiStatus status = transition_plan_begin(pipeline, &plan);
    if (status == UMI_STATUS_OK) status = transition_plan_tick(&plan, index, state);
    if (status == UMI_STATUS_OK) status = transition_plan_refresh(pipeline, &plan);
    if (status != UMI_STATUS_OK) return status;
    transition_plan_publish(pipeline, &plan);
    UmiDeveloperOperationSnapshot *operation = &pipeline->operations[index];
    operation->exit_code = exit_code;
    operation->has_exit_code = 1;
    if (state == UMI_DEVELOPER_OPERATION_SUCCEEDED)
        operation->progress_basis_points = 10000U;
    (void)copy_summary(operation, summary);
    return UMI_STATUS_OK;
}

/*
 * Provide the developer pipeline complete operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_pipeline_complete(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    int exit_code,
    const char *summary)
{
    return finish_operation(
        pipeline,
        operation_id,
        UMI_DEVELOPER_OPERATION_SUCCEEDED,
        exit_code,
        summary);
}

/*
 * Provide the developer pipeline fail operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_pipeline_fail(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    int exit_code,
    const char *summary)
{
    return finish_operation(
        pipeline,
        operation_id,
        UMI_DEVELOPER_OPERATION_FAILED,
        exit_code,
        summary);
}

/*
 * Provide the developer pipeline cancel operation used by this module and its client
 * applications.
 */
/* Retain direct cancellation for review. Cancellation and downstream blocking now share one checked model publication. */
#if 0
UmiStatus umi_developer_pipeline_cancel(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id,
    const char *summary)
{
    size_t index;
    UmiDeveloperOperationSnapshot *operation;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || operation_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!find_index(pipeline, operation_id, &index)) {
        return UMI_STATUS_NOT_FOUND;
    }

    operation = &pipeline->operations[index];
    /* Apply this branch only when its contract condition is satisfied. */
    if (operation->state != UMI_DEVELOPER_OPERATION_QUEUED &&
        operation->state != UMI_DEVELOPER_OPERATION_RUNNING &&
        operation->state != UMI_DEVELOPER_OPERATION_BLOCKED) {
        return UMI_STATUS_INVALID_STATE;
    }

    operation->state = UMI_DEVELOPER_OPERATION_CANCELLED;
    operation->has_exit_code = 0;
    (void)copy_summary(operation, summary);
    operation->revision += 1U;
    pipeline->revision += 1U;
    return umi_developer_pipeline_refresh(pipeline);
}
#endif

UmiStatus umi_developer_pipeline_cancel(
    UmiDeveloperPipeline *pipeline, const char *operation_id, const char *summary)
{
    if (pipeline == NULL || operation_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index;
    if (!find_index(pipeline, operation_id, &index)) return UMI_STATUS_NOT_FOUND;
    UmiDeveloperOperationState state = pipeline->operations[index].state;
    if (state != UMI_DEVELOPER_OPERATION_QUEUED &&
        state != UMI_DEVELOPER_OPERATION_RUNNING &&
        state != UMI_DEVELOPER_OPERATION_BLOCKED) return UMI_STATUS_INVALID_STATE;
    UmiDeveloperTransitionPlan plan;
    UmiStatus status = transition_plan_begin(pipeline, &plan);
    if (status == UMI_STATUS_OK)
        status = transition_plan_tick(&plan, index, UMI_DEVELOPER_OPERATION_CANCELLED);
    if (status == UMI_STATUS_OK) status = transition_plan_refresh(pipeline, &plan);
    if (status != UMI_STATUS_OK) return status;
    transition_plan_publish(pipeline, &plan);
    pipeline->operations[index].has_exit_code = 0;
    (void)copy_summary(&pipeline->operations[index], summary);
    return UMI_STATUS_OK;
}

/*
 * Provide the developer pipeline retry operation used by this module and its client
 * applications.
 */
/* Retain retry-before-cascade for review. Requeueing and dependent unblocking are admitted together before clearing the previous result. */
#if 0
UmiStatus umi_developer_pipeline_retry(
    UmiDeveloperPipeline *pipeline,
    const char *operation_id)
{
    size_t index;
    UmiDeveloperOperationSnapshot *operation;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || operation_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!find_index(pipeline, operation_id, &index)) {
        return UMI_STATUS_NOT_FOUND;
    }

    operation = &pipeline->operations[index];
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (operation->state != UMI_DEVELOPER_OPERATION_FAILED &&
        operation->state != UMI_DEVELOPER_OPERATION_CANCELLED) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (operation->attempt_count >= operation->max_attempts) {
        return UMI_STATUS_INVALID_STATE;
    }

    operation->state = UMI_DEVELOPER_OPERATION_QUEUED;
    operation->progress_basis_points = 0U;
    operation->exit_code = 0;
    operation->has_exit_code = 0;
    operation->summary[0] = '\0';
    operation->revision += 1U;
    pipeline->revision += 1U;
    return umi_developer_pipeline_refresh(pipeline);
}
#endif

UmiStatus umi_developer_pipeline_retry(
    UmiDeveloperPipeline *pipeline, const char *operation_id)
{
    if (pipeline == NULL || operation_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index;
    if (!find_index(pipeline, operation_id, &index)) return UMI_STATUS_NOT_FOUND;
    const UmiDeveloperOperationSnapshot *current = &pipeline->operations[index];
    if ((current->state != UMI_DEVELOPER_OPERATION_FAILED &&
         current->state != UMI_DEVELOPER_OPERATION_CANCELLED) ||
        current->attempt_count >= current->max_attempts) return UMI_STATUS_INVALID_STATE;
    UmiDeveloperTransitionPlan plan;
    UmiStatus status = transition_plan_begin(pipeline, &plan);
    if (status == UMI_STATUS_OK)
        status = transition_plan_tick(&plan, index, UMI_DEVELOPER_OPERATION_QUEUED);
    if (status == UMI_STATUS_OK) status = transition_plan_refresh(pipeline, &plan);
    if (status != UMI_STATUS_OK) return status;
    transition_plan_publish(pipeline, &plan);
    UmiDeveloperOperationSnapshot *operation = &pipeline->operations[index];
    operation->progress_basis_points = 0U;
    operation->exit_code = 0;
    operation->has_exit_code = 0;
    operation->summary[0] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Provide the developer pipeline snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_pipeline_snapshot(
    UmiDeveloperPipeline *pipeline,
    UmiDeveloperPipelineSnapshot *out_snapshot)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pipeline == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_pipeline_refresh(pipeline);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DEVELOPER_PIPELINE_API_VERSION;
    out_snapshot->operation_count = pipeline->operation_count;
    out_snapshot->dependency_count = pipeline->dependency_count;
    out_snapshot->revision = pipeline->revision;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < pipeline->operation_count; ++index) {
        UmiDeveloperOperationState state = pipeline->operations[index].state;
        /* Select the behaviour associated with the requested command or state value. */
        switch (state) {
            case UMI_DEVELOPER_OPERATION_QUEUED:
                out_snapshot->queued_count += 1U;
                /* Keep the operation inside its valid bounds before reading, writing or adding data. */
                if (dependencies_succeeded(pipeline, index)) {
                    out_snapshot->ready_count += 1U;
                }
                break;
            case UMI_DEVELOPER_OPERATION_RUNNING:
                out_snapshot->running_count += 1U;
                break;
            case UMI_DEVELOPER_OPERATION_SUCCEEDED:
                out_snapshot->succeeded_count += 1U;
                break;
            case UMI_DEVELOPER_OPERATION_FAILED:
                out_snapshot->failed_count += 1U;
                break;
            case UMI_DEVELOPER_OPERATION_CANCELLED:
                out_snapshot->cancelled_count += 1U;
                break;
            case UMI_DEVELOPER_OPERATION_BLOCKED:
                out_snapshot->blocked_count += 1U;
                break;
            default:
                return UMI_STATUS_INTERNAL_ERROR;
        }
    }

    return UMI_STATUS_OK;
}


/* Shared ready-start keeps the scheduler and runtime on one atomic model path.
 * The output is assigned only after successful publication. */
UmiStatus umi_developer_pipeline_start_next(UmiDeveloperPipeline *pipeline,
    UmiDeveloperOperationSnapshot *out_operation)
{
    UmiDeveloperTransitionPlan plan;
    size_t index = 0U;
    UmiStatus status = transition_plan_begin(pipeline, &plan);
    if (status == UMI_STATUS_OK) status = transition_plan_refresh(pipeline, &plan);
    if (status == UMI_STATUS_OK) status = transition_plan_next(pipeline, &plan, &index);
    if (status == UMI_STATUS_OK) status = transition_plan_start(pipeline, &plan, index);
    if (status != UMI_STATUS_OK) return status;
    transition_publish_start(pipeline, &plan, index);
    if (out_operation != NULL) *out_operation = pipeline->operations[index];
    return UMI_STATUS_OK;
}

/* This query does not reserve a lock or mutate identities. Under the required
 * single-owner, non-reentrant execution contract, the runtime calls it directly
 * before starting the selected operation and entering its external executor. */
UmiStatus umi_developer_pipeline_admit_next_execution(
    const UmiDeveloperPipeline *pipeline)
{
    UmiDeveloperTransitionPlan started;
    size_t index = 0U;
    UmiStatus status = transition_plan_begin(pipeline, &started);
    if (status == UMI_STATUS_OK) status = transition_plan_refresh(pipeline, &started);
    if (status == UMI_STATUS_OK) status = transition_plan_next(pipeline, &started, &index);
    if (status == UMI_STATUS_OK) status = transition_plan_start(pipeline, &started, index);
    if (status != UMI_STATUS_OK) return status;
    const UmiDeveloperOperationState outcomes[] = {
        UMI_DEVELOPER_OPERATION_SUCCEEDED, UMI_DEVELOPER_OPERATION_FAILED,
        UMI_DEVELOPER_OPERATION_CANCELLED
    };
    for (size_t i = 0U; i < sizeof(outcomes) / sizeof(outcomes[0]); ++i) {
        UmiDeveloperTransitionPlan finished = started;
        status = transition_plan_tick(&finished, index, outcomes[i]);
        if (status == UMI_STATUS_OK) status = transition_plan_refresh(pipeline, &finished);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}
