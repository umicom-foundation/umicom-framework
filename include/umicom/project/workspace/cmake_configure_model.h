/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/cmake_configure_model.h
 * PURPOSE: Framework-owned Cmake Configure Model contract and lifecycle state.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_CMAKE_CONFIGURE_MODEL_H
#define UMICOM_PROJECT_WORKSPACE_CMAKE_CONFIGURE_MODEL_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceCmakeConfigureModel {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceCmakeConfigureModel;
    UmiStatus umi_project_workspace_cmake_configure_model_init(UmiProjectWorkspaceCmakeConfigureModel *value,const char *id);
    UmiStatus umi_project_workspace_cmake_configure_model_validate(const UmiProjectWorkspaceCmakeConfigureModel *value);
    UmiStatus umi_project_workspace_cmake_configure_model_set_name(UmiProjectWorkspaceCmakeConfigureModel *value,const char *name);
    UmiStatus umi_project_workspace_cmake_configure_model_set_detail(UmiProjectWorkspaceCmakeConfigureModel *value,const char *detail);
    UmiStatus umi_project_workspace_cmake_configure_model_set_state(UmiProjectWorkspaceCmakeConfigureModel *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_cmake_configure_model_set_metric(UmiProjectWorkspaceCmakeConfigureModel *value,uint64_t metric);
    bool umi_project_workspace_cmake_configure_model_same_identity(const UmiProjectWorkspaceCmakeConfigureModel *left,const UmiProjectWorkspaceCmakeConfigureModel *right);
#ifdef __cplusplus
}
#endif
#endif
