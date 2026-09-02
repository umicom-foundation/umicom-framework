/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/source.c
 *
 * PURPOSE:
 *   Describe a retrievable source and its human-readable attribution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Source attribution is separated from generated text so UIs can show where retrieved evidence came from.
 */

#include "umicom/ai/source.h"
#include <stddef.h>

#include <string.h>

/*
 * Initialise ai source from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ai_source_init(UmiAiSource *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(value, 0, sizeof(*value));
    return UMI_STATUS_OK;
}
