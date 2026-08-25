/* Umicom Framework | Workspace Recent Projects | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_RECENT_PROJECTS_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_RECENT_PROJECTS_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceWorkspaceRecentProjects {
        char items[UMI_PROJECT_WORKSPACE_MEDIUM_CAPACITY][UMI_PROJECT_WORKSPACE_TEXT_CAPACITY];
        size_t count;
        uint64_t revision;
    }
    UmiProjectWorkspaceWorkspaceRecentProjects;
    void umi_project_workspace_workspace_recent_projects_init(UmiProjectWorkspaceWorkspaceRecentProjects *value);
    UmiStatus umi_project_workspace_workspace_recent_projects_add(UmiProjectWorkspaceWorkspaceRecentProjects *value, const char *text);
    bool umi_project_workspace_workspace_recent_projects_contains(const UmiProjectWorkspaceWorkspaceRecentProjects *value, const char *text);
    UmiStatus umi_project_workspace_workspace_recent_projects_remove(UmiProjectWorkspaceWorkspaceRecentProjects *value, const char *text);
#ifdef __cplusplus
}
#endif
#endif
