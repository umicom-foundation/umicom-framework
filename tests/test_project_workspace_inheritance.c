/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_project_workspace_inheritance.c
 *
 * PURPOSE:
 *   Implement the test project workspace inheritance behavior for
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
 * Batch 34 setting and variable inheritance tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/project/project.h"

int main(void)
{
    UmiProjectWorkspace *workspace = NULL;
    UmiProjectWorkspaceModel *model = NULL;
    UmiProjectDescriptorSnapshot project = {0};
    UmiProjectWorkspaceRootSnapshot root = {0};
    UmiProjectWorkspaceGroupSnapshot group = {0};
    UmiProjectWorkspaceMemberSnapshot member = {0};
    UmiProjectWorkspaceSettingSnapshot setting = {0};
    UmiProjectWorkspaceResolvedSetting resolved;
    UmiProjectVariableSnapshot variable = {0};

    if (umi_project_workspace_create(&workspace) != UMI_STATUS_OK) return 1;
    project.struct_size=(uint32_t)sizeof(project);
    project.api_version=UMI_PROJECT_DESCRIPTOR_API_VERSION;
    strcpy(project.id,"studio"); strcpy(project.name,"Studio");
    strcpy(project.root_uri,"/src/studio"); project.enabled=1;
    if (umi_project_descriptor_registry_upsert(
            umi_project_workspace_descriptor(workspace),&project)!=UMI_STATUS_OK)
        return 2;
    if (umi_project_workspace_model_create(workspace,&model)!=UMI_STATUS_OK)return 3;
    root.struct_size=(uint32_t)sizeof(root);
    root.api_version=UMI_PROJECT_WORKSPACE_MODEL_API_VERSION;
    strcpy(root.id,"main"); strcpy(root.path,"/src"); root.enabled=1;
    if(umi_project_workspace_model_upsert_root(model,&root)!=UMI_STATUS_OK)return 4;
    group.struct_size=(uint32_t)sizeof(group);
    group.api_version=UMI_PROJECT_WORKSPACE_GROUP_API_VERSION;
    strcpy(group.id,"apps"); strcpy(group.name,"Applications");
    group.enabled=1;
    if(umi_project_workspace_model_upsert_group(model,&group)!=UMI_STATUS_OK)return 5;
    member.struct_size=(uint32_t)sizeof(member);
    member.api_version=UMI_PROJECT_WORKSPACE_MODEL_API_VERSION;
    strcpy(member.id,"member.studio"); strcpy(member.root_id,"main");
    strcpy(member.project_id,"studio"); strcpy(member.group_id,"apps");
    member.enabled=1;
    if(umi_project_workspace_model_upsert_member(model,&member)!=UMI_STATUS_OK)return 6;

    setting.struct_size=(uint32_t)sizeof(setting);
    setting.api_version=UMI_PROJECT_WORKSPACE_SETTING_API_VERSION;
    strcpy(setting.id,"workspace.generator"); strcpy(setting.key,"generator");
    strcpy(setting.value,"Ninja");
    setting.scope=UMI_PROJECT_WORKSPACE_SETTING_WORKSPACE;
    if(umi_project_workspace_model_upsert_setting(model,&setting)!=UMI_STATUS_OK)return 7;
    strcpy(setting.id,"group.generator"); strcpy(setting.owner_id,"apps");
    strcpy(setting.value,"Ninja Multi-Config");
    setting.scope=UMI_PROJECT_WORKSPACE_SETTING_GROUP;
    if(umi_project_workspace_model_upsert_setting(model,&setting)!=UMI_STATUS_OK)return 8;
    if(umi_project_workspace_model_resolve_setting(
            model,"studio","generator",&resolved)!=UMI_STATUS_OK ||
       strcmp(resolved.value,"Ninja Multi-Config")!=0 ||
       resolved.source_scope!=UMI_PROJECT_WORKSPACE_SETTING_GROUP)return 9;
    strcpy(setting.id,"project.generator"); strcpy(setting.owner_id,"studio");
    strcpy(setting.value,"Ninja");
    setting.scope=UMI_PROJECT_WORKSPACE_SETTING_PROJECT;
    if(umi_project_workspace_model_upsert_setting(model,&setting)!=UMI_STATUS_OK)return 10;
    if(umi_project_workspace_model_resolve_setting(
            model,"studio","generator",&resolved)!=UMI_STATUS_OK ||
       strcmp(resolved.value,"Ninja")!=0 ||
       resolved.source_scope!=UMI_PROJECT_WORKSPACE_SETTING_PROJECT)return 11;

    variable.struct_size=(uint32_t)sizeof(variable);
    variable.api_version=UMI_PROJECT_VARIABLE_API_VERSION;
    strcpy(variable.id,"workspace.jobs"); strcpy(variable.name,"jobs");
    strcpy(variable.value,"2"); strcpy(variable.scope,"workspace");
    if(umi_project_variable_registry_upsert(
            umi_project_workspace_variable(workspace),&variable)!=UMI_STATUS_OK)
        return 12;
    strcpy(variable.id,"studio.jobs"); strcpy(variable.project_id,"studio");
    strcpy(variable.value,"4"); strcpy(variable.scope,"project");
    if(umi_project_variable_registry_upsert(
            umi_project_workspace_variable(workspace),&variable)!=UMI_STATUS_OK)
        return 13;
    if(umi_project_workspace_model_resolve_variable(
            model,"studio","jobs",&resolved)!=UMI_STATUS_OK ||
       strcmp(resolved.value,"4")!=0)return 14;

    umi_project_workspace_model_destroy(model);
    umi_project_workspace_destroy(workspace);
    return 0;
}
