/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/cmake_model.h
 *
 * PURPOSE:
 *   Publish the public cmake model contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_CMAKE_MODEL_H
#define UMICOM_PROJECT_WORKSPACE_CMAKE_MODEL_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceCmakeModel {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceCmakeModel;
    UmiStatus umi_project_workspace_cmake_model_init(UmiProjectWorkspaceCmakeModel *value,const char *id);
    UmiStatus umi_project_workspace_cmake_model_validate(const UmiProjectWorkspaceCmakeModel *value);
    UmiStatus umi_project_workspace_cmake_model_set_name(UmiProjectWorkspaceCmakeModel *value,const char *name);
    UmiStatus umi_project_workspace_cmake_model_set_detail(UmiProjectWorkspaceCmakeModel *value,const char *detail);
    UmiStatus umi_project_workspace_cmake_model_set_state(UmiProjectWorkspaceCmakeModel *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_cmake_model_set_metric(UmiProjectWorkspaceCmakeModel *value,uint64_t metric);
    bool umi_project_workspace_cmake_model_same_identity(const UmiProjectWorkspaceCmakeModel *left,const UmiProjectWorkspaceCmakeModel *right);
#ifdef __cplusplus
}
#endif
#endif
