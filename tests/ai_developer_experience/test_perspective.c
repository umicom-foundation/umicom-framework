/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_perspective.c
 *
 * PURPOSE:
 *   Contract coverage for AI Developer Experience perspective.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai_developer_experience/perspective.h"

int main(void)
{
    const UmiDeveloperWorkbenchPerspectiveDefinition *perspective =
        umi_ai_developer_perspective_definition();

    assert(perspective != NULL);
    assert(strcmp(
        perspective->perspective_id,
        "umicom.perspective.ai-developer") == 0);
    assert(perspective->surface_count == 12U);
    assert(strcmp(
        perspective->default_focus_surface_id,
        "umicom.ai-developer.chat") == 0);
    assert(umi_developer_workbench_perspective_validate(perspective) ==
           UMI_STATUS_OK);
    return 0;
}

