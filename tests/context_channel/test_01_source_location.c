/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/test_01_source_location.c
 *
 * PURPOSE:
 *   Exercise source location boundaries and stable status behaviour.
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
#include "umicom/context_channel/source_location.h"
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
    UmiSourceLocationContext context;
    umi_source_location_context_init(&context);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiSourceLocationContext, &context, revision,
        umi_source_location_context_set_workspace_id(&context, "workspace-1"));
    assert(umi_source_location_context_set_workspace_id(&context, "workspace-1") == UMI_STATUS_OK);
    assert(strcmp(context.workspace_id, "workspace-1") == 0);
    assert(umi_source_location_context_set_workspace_id(NULL, "workspace-1") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiSourceLocationContext, &context, revision,
        umi_source_location_context_set_file_path(&context, "src/main.c"));
    assert(umi_source_location_context_set_file_path(&context, "src/main.c") == UMI_STATUS_OK);
    assert(strcmp(context.file_path, "src/main.c") == 0);
    assert(umi_source_location_context_set_file_path(NULL, "src/main.c") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiSourceLocationContext, &context, revision,
        umi_source_location_context_set_symbol(&context, "main"));
    assert(umi_source_location_context_set_symbol(&context, "main") == UMI_STATUS_OK);
    assert(strcmp(context.symbol, "main") == 0);
    assert(umi_source_location_context_set_symbol(NULL, "main") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiSourceLocationContext, &context, revision,
        umi_source_location_context_set_line(&context, 12U));
    assert(umi_source_location_context_set_line(&context, 12U) == UMI_STATUS_OK);
    assert(context.line == 12U);
    assert(umi_source_location_context_set_line(NULL, 12U) == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiSourceLocationContext, &context, revision,
        umi_source_location_context_set_column(&context, 4U));
    assert(umi_source_location_context_set_column(&context, 4U) == UMI_STATUS_OK);
    assert(context.column == 4U);
    assert(umi_source_location_context_set_column(NULL, 4U) == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiSourceLocationContext, &context, revision,
        umi_source_location_context_set_selection_length(&context, 3U));
    assert(umi_source_location_context_set_selection_length(&context, 3U) == UMI_STATUS_OK);
    assert(context.selection_length == 3U);
    assert(umi_source_location_context_set_selection_length(NULL, 3U) == UMI_STATUS_INVALID_ARGUMENT);
    assert(context.revision == 7U);
    assert(umi_source_location_context_validate(&context) == UMI_STATUS_OK);
    return 0;
}
