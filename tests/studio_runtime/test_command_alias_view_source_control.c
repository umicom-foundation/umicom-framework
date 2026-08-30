/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_command_alias_view_source_control.c
 *
 * PURPOSE:
 *   Verify Studio command compatibility alias view.source-control.
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
#include "umicom/studio_runtime/command_aliases/view_source_control.h"

int main(void)
{
    const UmiStudioRuntimeCommandAliasDefinition *definition =
        umi_studio_command_alias_view_source_control();

    assert(definition != NULL);
    assert(strcmp(definition->alias_id, "view.source-control") == 0);
    assert(umi_studio_command_alias_validate(definition) == UMI_STATUS_OK);
    return 0;
}
