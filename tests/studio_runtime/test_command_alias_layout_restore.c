/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_command_alias_layout_restore.c
 *
 * PURPOSE:
 *   Verify Studio command compatibility alias layout.restore.
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
#include "umicom/studio_runtime/command_aliases/layout_restore.h"

int main(void)
{
    const UmiStudioRuntimeCommandAliasDefinition *definition =
        umi_studio_command_alias_layout_restore();

    assert(definition != NULL);
    assert(strcmp(definition->alias_id, "layout.restore") == 0);
    assert(umi_studio_command_alias_validate(definition) == UMI_STATUS_OK);
    return 0;
}
