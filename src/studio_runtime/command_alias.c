/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_alias.c
 *
 * PURPOSE:
 *   Validate Studio command compatibility aliases.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_alias.h"

UmiStatus umi_studio_command_alias_validate(
    const UmiStudioRuntimeCommandAliasDefinition *definition)
{
    if (definition == NULL ||
        definition->alias_id == NULL ||
        definition->alias_id[0] == '\0' ||
        definition->title == NULL ||
        definition->title[0] == '\0' ||
        definition->category == NULL ||
        definition->description == NULL ||
        (definition->activate_surface &&
         (definition->surface < UMI_STUDIO_SURFACE_EXPLORER ||
          definition->surface > UMI_STUDIO_SURFACE_LAST))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}
