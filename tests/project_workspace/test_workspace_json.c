/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_workspace_json.c
 *
 * PURPOSE:
 *   Implement the test workspace json behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/workspace_json.h"
#include "../value_archive/construction_cases.h"
#include <string.h>
#define CHECK(x) do{if(!(x))return __LINE__;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted workspace snapshot value. */
static int UmiProjectWorkspaceWorkspaceSnapshotConstructionCases(void)
{
    UmiProjectWorkspaceWorkspaceSnapshot *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_project_workspace_workspace_snapshot_init(value, "w") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->workspace_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_project_workspace_workspace_snapshot_init(value, oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_project_workspace_workspace_snapshot_init(value, value->workspace_id) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->workspace_id, "w") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiProjectWorkspaceWorkspaceSnapshotConstructionCases() != 0) return 1;

    UmiProjectWorkspaceWorkspaceSnapshot s;
    char j[512];
    CHECK(umi_project_workspace_workspace_snapshot_init(&s,"w")==UMI_STATUS_OK);
    s.project_count=3U;
    CHECK(umi_project_workspace_workspace_json_render(&s,j,sizeof(j))==UMI_STATUS_OK);
    CHECK(strstr(j,"\"projects\":3")!=NULL);
    return 0;
}
