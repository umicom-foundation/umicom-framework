/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_command_alias_document_save.c
 *
 * PURPOSE:
 *   Verify Studio command compatibility alias document.save.
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
#include "umicom/studio_runtime/command_aliases/document_save.h"

int main(void)
{
    const UmiStudioRuntimeCommandAliasDefinition *definition =
        umi_studio_command_alias_document_save();

    assert(definition != NULL);
    assert(strcmp(definition->alias_id, "document.save") == 0);
    assert(umi_studio_command_alias_validate(definition) == UMI_STATUS_OK);
    return 0;
}
