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
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the project workspace cmake install rule data shared with callers of this
 * public contract.
 */
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
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_project_workspace_cmake_install_rule_archive_encode(const UmiProjectWorkspaceCmakeInstallRule *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_project_workspace_cmake_install_rule_archive_decode(const void *bytes, size_t byte_count,
    UmiProjectWorkspaceCmakeInstallRule *value);

#ifdef __cplusplus
}
#endif
#endif
