/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_project_workspace_model.c
 *
 * PURPOSE:
 *   Implement the test project workspace model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Batch 34 multi-root workspace and deterministic dependency-order tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>

#include "umicom/project/project.h"

/*
 * Exercise add project and return a clear result when the behaviour no longer matches its
 * contract.
 */
static int add_project(UmiProjectWorkspace *workspace, const char *id,
                       const char *name, const char *root)
{
    UmiProjectDescriptorSnapshot project = {0};
    project.struct_size = (uint32_t)sizeof(project);
    project.api_version = UMI_PROJECT_DESCRIPTOR_API_VERSION;
    strcpy(project.id, id);
    strcpy(project.name, name);
    strcpy(project.root_uri, root);
    project.enabled = 1;
    return umi_project_descriptor_registry_upsert(
        umi_project_workspace_descriptor(workspace), &project) == UMI_STATUS_OK
        ? 0 : 1;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiProjectWorkspace *workspace = NULL;
    UmiProjectWorkspaceModel *model = NULL;
    UmiProjectWorkspaceRootSnapshot first = {0};
    UmiProjectWorkspaceRootSnapshot second = {0};
    UmiProjectWorkspaceMemberSnapshot framework_member = {0};
    UmiProjectWorkspaceMemberSnapshot studio_member = {0};
    UmiProjectWorkspaceGroupSnapshot group = {0};
    UmiProjectReferenceSnapshot reference = {0};
    UmiProjectWorkspaceModelSnapshot snapshot;
    UmiProjectWorkspaceOrderSnapshot order;
    UmiProjectDescriptorSnapshot active;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_project_workspace_create(&workspace) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (add_project(workspace, "framework", "Umicom Framework", "/src/framework") ||
        add_project(workspace, "studio", "Umicom Studio", "/src/studio")) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_project_workspace_model_create(workspace, &model) != UMI_STATUS_OK)
        return 3;

    first.struct_size = (uint32_t)sizeof(first);
    first.api_version = UMI_PROJECT_WORKSPACE_MODEL_API_VERSION;
    strcpy(first.id, "core"); strcpy(first.path, "/src");
    strcpy(first.label, "Core sources"); first.enabled = 1; first.order = 10;
    second = first; strcpy(second.id, "apps"); strcpy(second.path, "/apps");
    strcpy(second.label, "Applications"); second.order = 20;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_project_workspace_model_upsert_root(model, &first) != UMI_STATUS_OK ||
        umi_project_workspace_model_upsert_root(model, &second) != UMI_STATUS_OK)
        return 4;

    group.struct_size = (uint32_t)sizeof(group);
    group.api_version = UMI_PROJECT_WORKSPACE_GROUP_API_VERSION;
    strcpy(group.id, "development"); strcpy(group.name, "Development");
    group.enabled = 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_project_workspace_model_upsert_group(model, &group) != UMI_STATUS_OK)
        return 5;

    framework_member.struct_size = (uint32_t)sizeof(framework_member);
    framework_member.api_version = UMI_PROJECT_WORKSPACE_MODEL_API_VERSION;
    strcpy(framework_member.id, "member.framework");
    strcpy(framework_member.root_id, "core");
    strcpy(framework_member.project_id, "framework");
    strcpy(framework_member.group_id, "development");
    framework_member.enabled = 1; framework_member.order = 20;
    studio_member = framework_member;
    strcpy(studio_member.id, "member.studio");
    strcpy(studio_member.root_id, "apps");
    strcpy(studio_member.project_id, "studio");
    studio_member.order = 10;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_project_workspace_model_upsert_member(model, &framework_member) !=
            UMI_STATUS_OK ||
        umi_project_workspace_model_upsert_member(model, &studio_member) !=
            UMI_STATUS_OK) return 6;

    reference.struct_size = (uint32_t)sizeof(reference);
    reference.api_version = UMI_PROJECT_REFERENCE_API_VERSION;
    strcpy(reference.id, "studio.framework");
    strcpy(reference.project_id, "studio");
    strcpy(reference.target_project_id, "framework");
    strcpy(reference.kind, "build");
    reference.required = 1; reference.available = 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_project_reference_registry_upsert(
            umi_project_workspace_reference(workspace), &reference) !=
        UMI_STATUS_OK) return 7;

    /* Apply this operation only while the related capability or state is available. */
    if (umi_project_workspace_model_resolve_active_project(model, &active) !=
            UMI_STATUS_OK || strcmp(active.id, "studio") != 0) return 8;
    /* Apply this operation only while the related capability or state is available. */
    if (umi_project_workspace_model_set_active_project(model, "framework") !=
            UMI_STATUS_OK ||
        umi_project_workspace_model_resolve_active_project(model, &active) !=
            UMI_STATUS_OK || strcmp(active.id, "framework") != 0) return 9;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_project_workspace_model_resolve_build_order(model, &order) !=
            UMI_STATUS_OK || order.project_count != 2U || order.has_cycle ||
        strcmp(order.project_ids[0], "framework") != 0 ||
        strcmp(order.project_ids[1], "studio") != 0) return 10;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_project_workspace_model_snapshot(model, &snapshot) != UMI_STATUS_OK ||
        snapshot.root_count != 2U || snapshot.member_count != 2U ||
        snapshot.enabled_project_count != 2U || !snapshot.has_active_project)
        return 11;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_project_workspace_model_remove_root(model, "apps") !=
        UMI_STATUS_INVALID_STATE) return 12;

    umi_project_workspace_model_destroy(model);
    umi_project_workspace_destroy(workspace);
    return 0;
}
