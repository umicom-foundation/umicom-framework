/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/generation_plan.c
 *
 * PURPOSE:
 *   Render file paths/content from a project template into a reviewable plan.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/generation_plan.h"

#include <string.h>
#include <stdlib.h>
#include "umicom/platform/rooted_files.h"

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise developer project generation plan from caller-provided values so later
 * operations receive a known state.
 */
void umi_developer_project_generation_plan_init(
    UmiDeveloperProjectGenerationPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return;

    (void)memset(plan, 0, sizeof(*plan));
    plan->revision = 1U;
}

/*
 * Provide the developer project generation plan build operation used by this module and
 * its client applications.
 */
/* The former builder cleared the public output before rendering and could
 * expose a partial recipe after failure. Retain it for review. The private
 * candidate keeps the same rendering and revision behaviour while the public
 * boundary publishes one complete validated value. */
#if 0
UmiStatus umi_developer_project_generation_plan_build(
    const UmiDeveloperProjectTemplate *project_template,
    const UmiDeveloperProjectVariableSet *variables,
    const char *project_root,
    UmiDeveloperProjectGenerationPlan *out_plan)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (project_template == NULL || variables == NULL ||
        project_root == NULL || project_root[0] == '\0' ||
        out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_project_template_validate(project_template);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    umi_developer_project_generation_plan_init(out_plan);

    status = copy_text(
        out_plan->project_root,
        sizeof(out_plan->project_root),
        project_root);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = copy_text(
        out_plan->template_id,
        sizeof(out_plan->template_id),
        project_template->template_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (project_template->file_count >
        UMI_DEVELOPER_PROJECT_GENERATION_FILE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < project_template->file_count; ++index) {
        const UmiDeveloperProjectTemplateFile *source =
            &project_template->files[index];
        UmiDeveloperProjectGeneratedFile *destination =
            &out_plan->files[out_plan->file_count];

        status = umi_developer_project_render_text(
            source->relative_path,
            variables,
            destination->relative_path,
            sizeof(destination->relative_path));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        status = umi_developer_project_render_text(
            source->content_template,
            variables,
            destination->content,
            sizeof(destination->content));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        destination->executable = source->executable;
        destination->optional = source->optional;
        out_plan->total_content_bytes += strlen(destination->content);
        out_plan->file_count += 1U;
    }

    out_plan->revision += 1U;
    return umi_developer_project_generation_plan_validate(out_plan);
}
#endif

static UmiStatus build_candidate(
    const UmiDeveloperProjectTemplate *project_template,
    const UmiDeveloperProjectVariableSet *variables,
    const char *project_root,
    UmiDeveloperProjectGenerationPlan *out_plan)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (project_template == NULL || variables == NULL ||
        project_root == NULL || project_root[0] == '\0' ||
        out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_project_template_validate(project_template);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    umi_developer_project_generation_plan_init(out_plan);

    status = copy_text(
        out_plan->project_root,
        sizeof(out_plan->project_root),
        project_root);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = copy_text(
        out_plan->template_id,
        sizeof(out_plan->template_id),
        project_template->template_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (project_template->file_count >
        UMI_DEVELOPER_PROJECT_GENERATION_FILE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < project_template->file_count; ++index) {
        const UmiDeveloperProjectTemplateFile *source =
            &project_template->files[index];
        UmiDeveloperProjectGeneratedFile *destination =
            &out_plan->files[out_plan->file_count];

        status = umi_developer_project_render_text(
            source->relative_path,
            variables,
            destination->relative_path,
            sizeof(destination->relative_path));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        status = umi_developer_project_render_text(
            source->content_template,
            variables,
            destination->content,
            sizeof(destination->content));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        destination->executable = source->executable;
        destination->optional = source->optional;
        out_plan->total_content_bytes += strlen(destination->content);
        out_plan->file_count += 1U;
    }

    out_plan->revision += 1U;
    return umi_developer_project_generation_plan_validate(out_plan);
}

UmiStatus umi_developer_project_generation_plan_build(
    const UmiDeveloperProjectTemplate *project_template,
    const UmiDeveloperProjectVariableSet *variables,
    const char *project_root,
    UmiDeveloperProjectGenerationPlan *out_plan)
{
    if (project_template == NULL || variables == NULL || project_root == NULL ||
        project_root[0] == '\0' || out_plan == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_developer_project_template_validate(project_template);
    if (status != UMI_STATUS_OK) return status;
    status = umi_developer_project_variable_set_validate(variables);
    if (status != UMI_STATUS_OK) return status;

    /* A complete plan is larger than a typical native worker stack budget.
     * Keep its candidate on the heap and publish only after every rendered
     * path, content and duplicate check succeeds. Borrowed inputs can point
     * into the previous output because no output byte changes during reads. */
    UmiDeveloperProjectGenerationPlan *candidate = calloc(1U, sizeof *candidate);
    /* Framework declares OUT_OF_MEMORY. Preserve the earlier spelling below
     * for integration review; the active path keeps the same failure behavior. */
#if 0
    if (candidate == NULL) return UMI_STATUS_NO_MEMORY;
#else
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
#endif
    status = build_candidate(project_template, variables, project_root, candidate);
    if (status == UMI_STATUS_OK) memcpy(out_plan, candidate, sizeof *candidate);
    free(candidate);
    return status;
}

/*
 * Check that developer project generation plan satisfies its contract before another
 * service relies on it.
 */
/* The exact-spelling validator is retained for review. The active boundary
 * shares portable native names and refuses file/parent and ASCII aliases
 * before generation. This does not resolve actual filesystem objects. */
#if 0
UmiStatus umi_developer_project_generation_plan_validate(
    const UmiDeveloperProjectGenerationPlan *plan)
{
    size_t index;
    size_t other;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL ||
        memchr(plan->project_root, '\0', sizeof(plan->project_root)) == NULL ||
        memchr(plan->template_id, '\0', sizeof(plan->template_id)) == NULL ||
        plan->project_root[0] == '\0' ||
        plan->template_id[0] == '\0' ||
        plan->file_count == 0U ||
        plan->file_count > UMI_DEVELOPER_PROJECT_GENERATION_FILE_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Validate every member before pairwise comparisons read a later record. */
    for (index = 0U; index < plan->file_count; ++index) {
        if (memchr(plan->files[index].relative_path, '\0', sizeof(plan->files[index].relative_path)) == NULL ||
            memchr(plan->files[index].content, '\0', sizeof(plan->files[index].content)) == NULL)
            return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan->file_count; ++index) {
        const char *path = plan->files[index].relative_path;

        /* Apply this branch only when its contract condition is satisfied. */
        if (memchr(path, '\0', sizeof(plan->files[index].relative_path)) == NULL ||
            memchr(plan->files[index].content, '\0', sizeof(plan->files[index].content)) == NULL ||
            strchr(path, ':') != NULL || path[0] == '\0' ||
            path[0] == '/' || path[0] == '\\' ||
            strstr(path, "..") != NULL) {
            return UMI_STATUS_PERMISSION_DENIED;
        }

        /* Visit each bounded item once so every record receives the same rule. */
        for (other = index + 1U; other < plan->file_count; ++other) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(path, plan->files[other].relative_path) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }

    return UMI_STATUS_OK;
}
#endif

/* The initial mixed admission/comparison pass is retained for review.
 * Complete name admission precedes comparisons so a later malformed path
 * keeps the established refusal status. */
#if 0
UmiStatus umi_developer_project_generation_plan_validate(
    const UmiDeveloperProjectGenerationPlan *plan)
{
    size_t index;
    size_t other;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL ||
        memchr(plan->project_root, '\0', sizeof(plan->project_root)) == NULL ||
        memchr(plan->template_id, '\0', sizeof(plan->template_id)) == NULL ||
        plan->project_root[0] == '\0' ||
        plan->template_id[0] == '\0' ||
        plan->file_count == 0U ||
        plan->file_count > UMI_DEVELOPER_PROJECT_GENERATION_FILE_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Validate every member before pairwise comparisons read a later record. */
    for (index = 0U; index < plan->file_count; ++index) {
        if (memchr(plan->files[index].relative_path, '\0', sizeof(plan->files[index].relative_path)) == NULL ||
            memchr(plan->files[index].content, '\0', sizeof(plan->files[index].content)) == NULL)
            return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan->file_count; ++index) {
        const char *path = plan->files[index].relative_path;

        /* Apply this branch only when its contract condition is satisfied. */
        if (memchr(path, '\0', sizeof(plan->files[index].relative_path)) == NULL ||
            memchr(plan->files[index].content, '\0', sizeof(plan->files[index].content)) == NULL ||
            strchr(path, ':') != NULL || path[0] == '\0' ||
            path[0] == '/' || path[0] == '\\' ||
            strstr(path, "..") != NULL) {
            return UMI_STATUS_PERMISSION_DENIED;
        }

        /* Reuse portable native-name admission before comparing records. */
        UmiStatus name_status = UmiRootedFileValidateRelativePath(path);
        if (name_status != UMI_STATUS_OK) return name_status;

        /* Visit each bounded item once so every record receives the same rule. */
        for (other = index + 1U; other < plan->file_count; ++other) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            UmiRootedPathRelation relation;
            UmiStatus relation_status = UmiRootedFileCompareRelativePaths(
                path, plan->files[other].relative_path, &relation);
            if (relation_status != UMI_STATUS_OK) return relation_status;
            /* Every record is a file; it cannot also be another record's parent.
             * Use the same ASCII case/separator portability rule on every host. */
            if (relation != UMI_ROOTED_PATH_DISJOINT)
                return UMI_STATUS_ALREADY_EXISTS;
        }
    }

    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_developer_project_generation_plan_validate(
    const UmiDeveloperProjectGenerationPlan *plan)
{
    if (plan == NULL ||
        memchr(plan->project_root, '\0', sizeof plan->project_root) == NULL ||
        memchr(plan->template_id, '\0', sizeof plan->template_id) == NULL ||
        plan->project_root[0] == '\0' || plan->template_id[0] == '\0' ||
        plan->file_count == 0U ||
        plan->file_count > UMI_DEVELOPER_PROJECT_GENERATION_FILE_CAPACITY)
        return UMI_STATUS_INVALID_ARGUMENT;

    /* Admit every owned string before comparison can read a later record. */
    for (size_t index = 0U; index < plan->file_count; ++index) {
        if (memchr(plan->files[index].relative_path, '\0',
                sizeof plan->files[index].relative_path) == NULL ||
            memchr(plan->files[index].content, '\0',
                sizeof plan->files[index].content) == NULL)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Complete name admission first. A malformed later path retains its own
     * refusal status instead of being interpreted during a duplicate check. */
    for (size_t index = 0U; index < plan->file_count; ++index) {
        const char *file_path = plan->files[index].relative_path;
        if (strchr(file_path, ':') != NULL || file_path[0] == '\0' ||
            file_path[0] == '/' || file_path[0] == '\\' ||
            strstr(file_path, "..") != NULL)
            return UMI_STATUS_PERMISSION_DENIED;
        UmiStatus status = UmiRootedFileValidateRelativePath(file_path);
        if (status != UMI_STATUS_OK) return status;
    }
    for (size_t index = 0U; index < plan->file_count; ++index) {
        for (size_t other = index + 1U; other < plan->file_count; ++other) {
            UmiRootedPathRelation relation;
            UmiStatus status = UmiRootedFileCompareRelativePaths(
                plan->files[index].relative_path,
                plan->files[other].relative_path, &relation);
            if (status != UMI_STATUS_OK) return status;
            /* Every record is a file, so it cannot also be another file's parent.
             * ASCII case/separator portability is the same on every host. */
            if (relation != UMI_ROOTED_PATH_DISJOINT)
                return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    return UMI_STATUS_OK;
}
