/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/cmake_preset.h
 * PURPOSE: Framework-owned Cmake Preset contract and lifecycle state.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_CMAKE_PRESET_H
#define UMICOM_PROJECT_WORKSPACE_CMAKE_PRESET_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceCmakePreset {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceCmakePreset;
    UmiStatus umi_project_workspace_cmake_preset_init(UmiProjectWorkspaceCmakePreset *value,const char *id);
    UmiStatus umi_project_workspace_cmake_preset_validate(const UmiProjectWorkspaceCmakePreset *value);
    UmiStatus umi_project_workspace_cmake_preset_set_name(UmiProjectWorkspaceCmakePreset *value,const char *name);
    UmiStatus umi_project_workspace_cmake_preset_set_detail(UmiProjectWorkspaceCmakePreset *value,const char *detail);
    UmiStatus umi_project_workspace_cmake_preset_set_state(UmiProjectWorkspaceCmakePreset *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_cmake_preset_set_metric(UmiProjectWorkspaceCmakePreset *value,uint64_t metric);
    bool umi_project_workspace_cmake_preset_same_identity(const UmiProjectWorkspaceCmakePreset *left,const UmiProjectWorkspaceCmakePreset *right);
#ifdef __cplusplus
}
#endif
#endif
