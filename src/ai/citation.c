/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/citation.c
 *
 * PURPOSE:
 *   Describe one citation from generated or retrieved content back to a source.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A citation keeps source identity and score explicit rather than hiding provenance inside generated prose.
 */

#include "umicom/ai/citation.h"
#include <stddef.h>

#include <string.h>

UmiStatus umi_ai_citation_init(UmiAiCitation *value)
{
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(value, 0, sizeof(*value));
    return UMI_STATUS_OK;
}
