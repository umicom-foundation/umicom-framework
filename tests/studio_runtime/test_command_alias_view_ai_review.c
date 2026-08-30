/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_command_alias_view_ai_review.c
 *
 * PURPOSE:
 *   Verify Studio command compatibility alias view.ai-review.
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
#include "umicom/studio_runtime/command_aliases/view_ai_review.h"

int main(void)
{
    const UmiStudioRuntimeCommandAliasDefinition *definition =
        umi_studio_command_alias_view_ai_review();

    assert(definition != NULL);
    assert(strcmp(definition->alias_id, "view.ai-review") == 0);
    assert(umi_studio_command_alias_validate(definition) == UMI_STATUS_OK);
    return 0;
}
