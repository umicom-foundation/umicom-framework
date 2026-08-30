/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace/cmake_install_rule.h
 *
 * PURPOSE:
 *   Publish the public cmake install rule contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_CMAKE_INSTALL_RULE_H
#define UMICOM_PROJECT_WORKSPACE_CMAKE_INSTALL_RULE_H
#include "umicom/project/workspace/types.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiProjectWorkspaceCmakeInstallRule {
        UmiProjectWorkspaceNamedState base;
        uint64_t metric;
    }
    UmiProjectWorkspaceCmakeInstallRule;
    UmiStatus umi_project_workspace_cmake_install_rule_init(UmiProjectWorkspaceCmakeInstallRule *value,const char *id);
    UmiStatus umi_project_workspace_cmake_install_rule_validate(const UmiProjectWorkspaceCmakeInstallRule *value);
    UmiStatus umi_project_workspace_cmake_install_rule_set_name(UmiProjectWorkspaceCmakeInstallRule *value,const char *name);
    UmiStatus umi_project_workspace_cmake_install_rule_set_detail(UmiProjectWorkspaceCmakeInstallRule *value,const char *detail);
    UmiStatus umi_project_workspace_cmake_install_rule_set_state(UmiProjectWorkspaceCmakeInstallRule *value,UmiProjectWorkspaceState state);
    void umi_project_workspace_cmake_install_rule_set_metric(UmiProjectWorkspaceCmakeInstallRule *value,uint64_t metric);
    bool umi_project_workspace_cmake_install_rule_same_identity(const UmiProjectWorkspaceCmakeInstallRule *left,const UmiProjectWorkspaceCmakeInstallRule *right);
#ifdef __cplusplus
}
#endif
#endif
