/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_source_control_workspace_bridge.c
 *
 * PURPOSE:
 *   Implement the test source control workspace bridge behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Framework source-control registry/workspace bridge test. Sammy Hegab, Umicom Foundation. MIT. */
#include <assert.h>
#include "umicom/umicom.h"
int main(void)
{
    UmiSourceControlService *service = NULL; UmiSourceControlServiceSnapshot snapshot;
    char temporary[UMI_PATH_CAPACITY]; char root[UMI_PATH_CAPACITY];
    assert(umi_fs_temp_directory(temporary, sizeof(temporary)) == UMI_STATUS_OK);
    assert(umi_fs_join(root, sizeof(root), temporary, "umicom-vcs-bridge") == UMI_STATUS_OK);
    (void)umi_fs_remove_tree(root); assert(umi_fs_make_directories(root) == UMI_STATUS_OK);
    assert(umi_source_control_service_create(&service) == UMI_STATUS_OK);
    assert(umi_source_control_service_open_workspace(service, root) == UMI_STATUS_OK);
    assert(umi_source_control_service_workspace(service) != NULL);
    assert(umi_source_control_service_snapshot(service, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.api_version == 3U && snapshot.workspace_open && !snapshot.workspace.available);
    assert(snapshot.workspace_coordinator_open);
    assert(umi_source_control_service_workspace_coordinator(service) != NULL);
    umi_source_control_service_close_workspace(service); assert(umi_source_control_service_workspace(service) == NULL);
    assert(umi_source_control_service_workspace_coordinator(service) == NULL);
    umi_source_control_service_destroy(service); assert(umi_fs_remove_tree(root) == UMI_STATUS_OK); return 0;
}
