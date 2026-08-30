/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/workspace_multi_root.h
 *
 * PURPOSE:
 *   Publish the public workspace multi root contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_WORKSPACE_MULTI_ROOT_H
#define UMICOM_PROJECT_WORKSPACE_WORKSPACE_MULTI_ROOT_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceWorkspaceMultiRoot {
        char roots[UMI_PROJECT_WORKSPACE_SMALL_CAPACITY][UMI_PROJECT_WORKSPACE_PATH_CAPACITY];
        size_t count;
    }
    UmiProjectWorkspaceWorkspaceMultiRoot;
    void umi_project_workspace_workspace_multi_root_init(UmiProjectWorkspaceWorkspaceMultiRoot *value);
    UmiStatus umi_project_workspace_workspace_multi_root_add_root(UmiProjectWorkspaceWorkspaceMultiRoot *value,const char *path);
    bool umi_project_workspace_workspace_multi_root_valid(const UmiProjectWorkspaceWorkspaceMultiRoot *value);
#ifdef __cplusplus
}
#endif
#endif
