/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/project_variant.h
 * PURPOSE: Framework-owned Project Variant contract and lifecycle state.
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_PROJECT_VARIANT_H
#define UMICOM_PROJECT_WORKSPACE_PROJECT_VARIANT_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceProjectVariant {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceProjectVariant;
    UmiStatus umi_project_workspace_project_variant_init(UmiProjectWorkspaceProjectVariant *value,const char *id);
    UmiStatus umi_project_workspace_project_variant_validate(const UmiProjectWorkspaceProjectVariant *value);
    UmiStatus umi_project_workspace_project_variant_set_name(UmiProjectWorkspaceProjectVariant *value,const char *name);
    UmiStatus umi_project_workspace_project_variant_set_detail(UmiProjectWorkspaceProjectVariant *value,const char *detail);
    UmiStatus umi_project_workspace_project_variant_set_state(UmiProjectWorkspaceProjectVariant *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_project_variant_set_metric(UmiProjectWorkspaceProjectVariant *value,uint64_t metric);
    bool umi_project_workspace_project_variant_same_identity(const UmiProjectWorkspaceProjectVariant *left,const UmiProjectWorkspaceProjectVariant *right);
#ifdef __cplusplus
}
#endif
#endif
