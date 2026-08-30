/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/application_feature_selection.h
 *
 * PURPOSE:
 *   Publish the public application feature selection contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_APPLICATION_FEATURE_SELECTION_H
#define UMICOM_PROJECT_WORKSPACE_APPLICATION_FEATURE_SELECTION_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceApplicationFeatureSelection {
        char items[UMI_PROJECT_WORKSPACE_MEDIUM_CAPACITY][UMI_PROJECT_WORKSPACE_TEXT_CAPACITY];
        size_t count;
        uint64_t revision;
    }
    UmiProjectWorkspaceApplicationFeatureSelection;
    void umi_project_workspace_application_feature_selection_init(UmiProjectWorkspaceApplicationFeatureSelection *value);
    UmiStatus umi_project_workspace_application_feature_selection_add(UmiProjectWorkspaceApplicationFeatureSelection *value, const char *text);
    bool umi_project_workspace_application_feature_selection_contains(const UmiProjectWorkspaceApplicationFeatureSelection *value, const char *text);
    UmiStatus umi_project_workspace_application_feature_selection_remove(UmiProjectWorkspaceApplicationFeatureSelection *value, const char *text);
#ifdef __cplusplus
}
#endif
#endif
