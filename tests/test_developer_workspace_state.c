/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_workspace_state.c
 *
 * PURPOSE:
 *   Verify developer workspace state codecs and atomic file persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/workspace_state.h"
#include "umicom/platform/filesystem.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    UmiDeveloperWorkspaceStateSnapshot input, decoded, loaded;
    char encoded[UMI_DEVELOPER_WORKSPACE_STATE_TEXT_CAPACITY];
    char temp[UMI_PATH_CAPACITY];
    char path[UMI_PATH_CAPACITY];

    umi_developer_workspace_state_init(&input);
    strcpy(input.context.project_id, "studio|main");
    strcpy(input.context.active_document, "C:%\\Dev\\one|two.c");
    strcpy(input.last_task_id, "build.all");
    strcpy(input.last_launch_profile_id, "studio.console");
    input.configure_before_build = 1;
    input.build_before_run = 1;
    input.test_after_build = 1;
    input.revision = 42U;
    assert(umi_developer_workspace_state_encode(&input, encoded, sizeof(encoded), NULL) == UMI_STATUS_OK);
    assert(umi_developer_workspace_state_decode(encoded, &decoded) == UMI_STATUS_OK);
    assert(strcmp(decoded.context.project_id, input.context.project_id) == 0);
    assert(strcmp(decoded.context.active_document, input.context.active_document) == 0);
    assert(decoded.revision == 42U);

    assert(umi_fs_temp_directory(temp, sizeof(temp)) == UMI_STATUS_OK);
    assert(umi_fs_join(path, sizeof(path), temp, "umicom-developer-state-test.txt") == UMI_STATUS_OK);
    assert(umi_developer_workspace_state_save(path, &input) == UMI_STATUS_OK);
    assert(umi_developer_workspace_state_load(path, &loaded) == UMI_STATUS_OK);
    assert(strcmp(loaded.last_task_id, "build.all") == 0);
    (void)remove(path);
    return 0;
}
