/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/authorengine.c
 *
 * PURPOSE:
 *   Configure the Umicom AuthorEngine executable and build deterministic CLI arguments.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * AuthorEngine is integrated through a process boundary, so Studio can reuse its capabilities without copying publishing logic into the IDE.
 */

#include "umicom/ai/authorengine.h"
#include <stddef.h>

UmiStatus umi_ai_authorengine_validate(const UmiAiAuthorEngineConfig *config)
{
    if (config == NULL || config->executable[0] == '\0' || config->workspace[0] == '\0' || config->provider[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
