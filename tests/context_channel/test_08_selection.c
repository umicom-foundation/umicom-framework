/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/test_08_selection.c
 *
 * PURPOSE:
 *   Exercise selection boundaries and stable status behaviour.
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
#include "umicom/context_channel/selection.h"
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
    UmiSelectionContext context;
    umi_selection_context_init(&context);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiSelectionContext, &context, revision,
        umi_selection_context_set_selection_id(&context, "selection-1"));
    assert(umi_selection_context_set_selection_id(&context, "selection-1") == UMI_STATUS_OK);
    assert(strcmp(context.selection_id, "selection-1") == 0);
    assert(umi_selection_context_set_selection_id(NULL, "selection-1") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiSelectionContext, &context, revision,
        umi_selection_context_set_selection_type(&context, "media"));
    assert(umi_selection_context_set_selection_type(&context, "media") == UMI_STATUS_OK);
    assert(strcmp(context.selection_type, "media") == 0);
    assert(umi_selection_context_set_selection_type(NULL, "media") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiSelectionContext, &context, revision,
        umi_selection_context_set_primary_id(&context, "asset-1"));
    assert(umi_selection_context_set_primary_id(&context, "asset-1") == UMI_STATUS_OK);
    assert(strcmp(context.primary_id, "asset-1") == 0);
    assert(umi_selection_context_set_primary_id(NULL, "asset-1") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiSelectionContext, &context, revision,
        umi_selection_context_set_secondary_id(&context, "track-1"));
    assert(umi_selection_context_set_secondary_id(&context, "track-1") == UMI_STATUS_OK);
    assert(strcmp(context.secondary_id, "track-1") == 0);
    assert(umi_selection_context_set_secondary_id(NULL, "track-1") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiSelectionContext, &context, revision,
        umi_selection_context_set_index(&context, 2U));
    assert(umi_selection_context_set_index(&context, 2U) == UMI_STATUS_OK);
    assert(context.index == 2U);
    assert(umi_selection_context_set_index(NULL, 2U) == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiSelectionContext, &context, revision,
        umi_selection_context_set_count(&context, 4U));
    assert(umi_selection_context_set_count(&context, 4U) == UMI_STATUS_OK);
    assert(context.count == 4U);
    assert(umi_selection_context_set_count(NULL, 4U) == UMI_STATUS_INVALID_ARGUMENT);
    assert(context.revision == 7U);
    assert(umi_selection_context_validate(&context) == UMI_STATUS_OK);
    return 0;
}
