/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/diagnostic_parser.c
 *
 * PURPOSE:
 *   Validate normalized diagnostic parser providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/diagnostic_parser.h"

UmiStatus umi_developer_diagnostic_parser_validate(
    const UmiDeveloperDiagnosticParser *parser)
{
    if (parser == NULL ||
        parser->structure_size != sizeof(*parser) ||
        parser->api_version != UMI_DEVELOPER_PRODUCTIVITY_API_VERSION ||
        parser->parser_id == NULL || parser->parser_id[0] == '\0' ||
        parser->title == NULL || parser->title[0] == '\0' ||
        parser->tool_id == NULL || parser->tool_id[0] == '\0' ||
        parser->parse == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}
