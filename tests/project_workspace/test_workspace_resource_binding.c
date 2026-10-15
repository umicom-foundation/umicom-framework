/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_workspace_resource_binding.c
 *
 * PURPOSE:
 *   Implement the test workspace resource binding behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/workspace_resource_binding.h"
#include "../value_archive/construction_cases.h"
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted workspace resource binding value. */
static int UmiProjectWorkspaceWorkspaceResourceBindingConstructionCases(void)
{
    UmiProjectWorkspaceWorkspaceResourceBinding *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_project_workspace_workspace_resource_binding_init(value, "project", "profile") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->owner_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_project_workspace_workspace_resource_binding_init(value, oversized, "profile") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_project_workspace_workspace_resource_binding_init(value, value->owner_id, "profile") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->owner_id, "project") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->target_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_project_workspace_workspace_resource_binding_init(value, "project", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_project_workspace_workspace_resource_binding_init(value, "project", value->target_id) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->target_id, "profile") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiProjectWorkspaceWorkspaceResourceBindingConstructionCases() != 0) return 1;

    UmiProjectWorkspaceWorkspaceResourceBinding v;
    CHECK(umi_project_workspace_workspace_resource_binding_init(&v,"project","profile")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_resource_binding_matches(&v,"project","profile"));
    CHECK(umi_project_workspace_workspace_resource_binding_retarget(&v,"profile2")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_resource_binding_matches(&v,"project","profile2"));
    return 0;
}
