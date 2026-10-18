/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/test_06_workspace.c
 *
 * PURPOSE:
 *   Exercise workspace boundaries and stable status behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../value_archive/revision_cases.h"
#include <assert.h>
#include <string.h>
#include "umicom/context_channel/workspace.h"
#include "umicom/context_channel/types.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    char destination[32];
    assert(umi_context_copy_text(destination,sizeof(destination),"context") == UMI_STATUS_OK);
    assert(strcmp(destination,"context") == 0);
    assert(umi_context_text_is_valid(destination,sizeof(destination)));
    assert(umi_context_kind_text(UMI_CONTEXT_KIND_INSTRUMENT) != NULL);
    assert(umi_context_colour_text(UMI_CONTEXT_COLOUR_BLUE) != NULL);

    /* Exercise the context owner itself, including its revision publication. */
    UmiWorkspaceContext context;
    umi_workspace_context_init(&context);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiWorkspaceContext, &context, revision,
        umi_workspace_context_set_workspace_id(&context, "workspace-1"));
    assert(umi_workspace_context_set_workspace_id(&context, "workspace-1") == UMI_STATUS_OK);
    assert(strcmp(context.workspace_id, "workspace-1") == 0);
    assert(umi_workspace_context_set_workspace_id(NULL, "workspace-1") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiWorkspaceContext, &context, revision,
        umi_workspace_context_set_profile_id(&context, "profile-1"));
    assert(umi_workspace_context_set_profile_id(&context, "profile-1") == UMI_STATUS_OK);
    assert(strcmp(context.profile_id, "profile-1") == 0);
    assert(umi_workspace_context_set_profile_id(NULL, "profile-1") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiWorkspaceContext, &context, revision,
        umi_workspace_context_set_root_path(&context, "projects/example"));
    assert(umi_workspace_context_set_root_path(&context, "projects/example") == UMI_STATUS_OK);
    assert(strcmp(context.root_path, "projects/example") == 0);
    assert(umi_workspace_context_set_root_path(NULL, "projects/example") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiWorkspaceContext, &context, revision,
        umi_workspace_context_set_perspective_id(&context, "editor"));
    assert(umi_workspace_context_set_perspective_id(&context, "editor") == UMI_STATUS_OK);
    assert(strcmp(context.perspective_id, "editor") == 0);
    assert(umi_workspace_context_set_perspective_id(NULL, "editor") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiWorkspaceContext, &context, revision,
        umi_workspace_context_set_layout_id(&context, "default"));
    assert(umi_workspace_context_set_layout_id(&context, "default") == UMI_STATUS_OK);
    assert(strcmp(context.layout_id, "default") == 0);
    assert(umi_workspace_context_set_layout_id(NULL, "default") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiWorkspaceContext, &context, revision,
        umi_workspace_context_set_trusted(&context, true));
    assert(umi_workspace_context_set_trusted(&context, true) == UMI_STATUS_OK);
    assert(context.trusted == true);
    assert(umi_workspace_context_set_trusted(NULL, true) == UMI_STATUS_INVALID_ARGUMENT);
    assert(context.revision == 7U);
    assert(umi_workspace_context_validate(&context) == UMI_STATUS_OK);
    return 0;
}
