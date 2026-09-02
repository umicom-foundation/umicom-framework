/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/application_frontend_selection.h
 *
 * PURPOSE:
 *   Publish the public application frontend selection contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_APPLICATION_FRONTEND_SELECTION_H
#define UMICOM_PROJECT_WORKSPACE_APPLICATION_FRONTEND_SELECTION_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace application frontend selection data shared with callers
 * of this public contract.
 */
    typedef struct UmiProjectWorkspaceApplicationFrontendSelection {
        char items[UMI_PROJECT_WORKSPACE_MEDIUM_CAPACITY][UMI_PROJECT_WORKSPACE_TEXT_CAPACITY];
        size_t count;
        uint64_t revision;
    }
    UmiProjectWorkspaceApplicationFrontendSelection;
    void umi_project_workspace_application_frontend_selection_init(UmiProjectWorkspaceApplicationFrontendSelection *value);
    UmiStatus umi_project_workspace_application_frontend_selection_add(UmiProjectWorkspaceApplicationFrontendSelection *value, const char *text);
    bool umi_project_workspace_application_frontend_selection_contains(const UmiProjectWorkspaceApplicationFrontendSelection *value, const char *text);
    UmiStatus umi_project_workspace_application_frontend_selection_remove(UmiProjectWorkspaceApplicationFrontendSelection *value, const char *text);
#ifdef __cplusplus
}
#endif
#endif
