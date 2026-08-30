/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/cmake_target.h
 *
 * PURPOSE:
 *   Publish the public cmake target contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_CMAKE_TARGET_H
#define UMICOM_PROJECT_WORKSPACE_CMAKE_TARGET_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceCmakeTarget {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceCmakeTarget;
    UmiStatus umi_project_workspace_cmake_target_init(UmiProjectWorkspaceCmakeTarget *value,const char *id);
    UmiStatus umi_project_workspace_cmake_target_validate(const UmiProjectWorkspaceCmakeTarget *value);
    UmiStatus umi_project_workspace_cmake_target_set_name(UmiProjectWorkspaceCmakeTarget *value,const char *name);
    UmiStatus umi_project_workspace_cmake_target_set_detail(UmiProjectWorkspaceCmakeTarget *value,const char *detail);
    UmiStatus umi_project_workspace_cmake_target_set_state(UmiProjectWorkspaceCmakeTarget *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_cmake_target_set_metric(UmiProjectWorkspaceCmakeTarget *value,uint64_t metric);
    bool umi_project_workspace_cmake_target_same_identity(const UmiProjectWorkspaceCmakeTarget *left,const UmiProjectWorkspaceCmakeTarget *right);
#ifdef __cplusplus
}
#endif
#endif
