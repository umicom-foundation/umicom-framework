/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_command_alias_view_build_dashboard.c
 *
 * PURPOSE:
 *   Verify Studio command compatibility alias view.build-dashboard.
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
#include "umicom/studio_runtime/command_aliases/view_build_dashboard.h"

int main(void)
{
    const UmiStudioRuntimeCommandAliasDefinition *definition =
        umi_studio_command_alias_view_build_dashboard();

    assert(definition != NULL);
    assert(strcmp(definition->alias_id, "view.build-dashboard") == 0);
    assert(umi_studio_command_alias_validate(definition) == UMI_STATUS_OK);
    return 0;
}
