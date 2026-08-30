/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/cmake_dependency.h
 *
 * PURPOSE:
 *   Publish the public cmake dependency contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_CMAKE_DEPENDENCY_H
#define UMICOM_PROJECT_WORKSPACE_CMAKE_DEPENDENCY_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceCmakeDependency {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceCmakeDependency;
    UmiStatus umi_project_workspace_cmake_dependency_init(UmiProjectWorkspaceCmakeDependency *value,const char *id);
    UmiStatus umi_project_workspace_cmake_dependency_validate(const UmiProjectWorkspaceCmakeDependency *value);
    UmiStatus umi_project_workspace_cmake_dependency_set_name(UmiProjectWorkspaceCmakeDependency *value,const char *name);
    UmiStatus umi_project_workspace_cmake_dependency_set_detail(UmiProjectWorkspaceCmakeDependency *value,const char *detail);
    UmiStatus umi_project_workspace_cmake_dependency_set_state(UmiProjectWorkspaceCmakeDependency *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_cmake_dependency_set_metric(UmiProjectWorkspaceCmakeDependency *value,uint64_t metric);
    bool umi_project_workspace_cmake_dependency_same_identity(const UmiProjectWorkspaceCmakeDependency *left,const UmiProjectWorkspaceCmakeDependency *right);
#ifdef __cplusplus
}
#endif
#endif
