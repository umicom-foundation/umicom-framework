/* Umicom Framework | Project Configuration Set | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_CONFIGURATION_SET_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_CONFIGURATION_SET_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceProjectConfigurationSet {
        char items[UMI_PROJECT_WORKSPACE_MEDIUM_CAPACITY][UMI_PROJECT_WORKSPACE_TEXT_CAPACITY];
        size_t count;
        uint64_t revision;
    }
    UmiProjectWorkspaceProjectConfigurationSet;
    void umi_project_workspace_project_configuration_set_init(UmiProjectWorkspaceProjectConfigurationSet *value);
    UmiStatus umi_project_workspace_project_configuration_set_add(UmiProjectWorkspaceProjectConfigurationSet *value, const char *text);
    bool umi_project_workspace_project_configuration_set_contains(const UmiProjectWorkspaceProjectConfigurationSet *value, const char *text);
    UmiStatus umi_project_workspace_project_configuration_set_remove(UmiProjectWorkspaceProjectConfigurationSet *value, const char *text);
#ifdef __cplusplus
}
#endif
#endif
