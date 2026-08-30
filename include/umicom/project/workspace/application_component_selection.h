/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/application_component_selection.h
 *
 * PURPOSE:
 *   Publish the public application component selection contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_APPLICATION_COMPONENT_SELECTION_H
#define UMICOM_PROJECT_WORKSPACE_APPLICATION_COMPONENT_SELECTION_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceApplicationComponentSelection {
        char items[UMI_PROJECT_WORKSPACE_MEDIUM_CAPACITY][UMI_PROJECT_WORKSPACE_TEXT_CAPACITY];
        size_t count;
        uint64_t revision;
    }
    UmiProjectWorkspaceApplicationComponentSelection;
    void umi_project_workspace_application_component_selection_init(UmiProjectWorkspaceApplicationComponentSelection *value);
    UmiStatus umi_project_workspace_application_component_selection_add(UmiProjectWorkspaceApplicationComponentSelection *value, const char *text);
    bool umi_project_workspace_application_component_selection_contains(const UmiProjectWorkspaceApplicationComponentSelection *value, const char *text);
    UmiStatus umi_project_workspace_application_component_selection_remove(UmiProjectWorkspaceApplicationComponentSelection *value, const char *text);
#ifdef __cplusplus
}
#endif
#endif
