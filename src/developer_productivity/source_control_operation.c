/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/source_control_operation.c
 *
 * PURPOSE:
 *   Validate source-control operation metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/source_control_operation.h"

UmiStatus umi_developer_source_control_operation_validate(
    const UmiDeveloperSourceControlOperationDefinition *definition)
{
    if (definition == NULL ||
        definition->structure_size != sizeof(*definition) ||
        definition->api_version != UMI_DEVELOPER_PRODUCTIVITY_API_VERSION ||
        definition->operation_id == NULL ||
        definition->operation_id[0] == '\0' ||
        definition->title == NULL ||
        definition->title[0] == '\0' ||
        definition->description == NULL ||
        definition->kind < UMI_DEVELOPER_SOURCE_CONTROL_REFRESH ||
        definition->kind > UMI_DEVELOPER_SOURCE_CONTROL_DIFF) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}
