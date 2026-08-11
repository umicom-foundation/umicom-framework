/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/source.c
 *
 * PURPOSE:
 *   Describe a retrievable source and its human-readable attribution.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Source attribution is separated from generated text so UIs can show where retrieved evidence came from.
 */

#include "umicom/ai/source.h"
#include <stddef.h>

#include <string.h>

UmiStatus umi_ai_source_init(UmiAiSource *value)
{
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(value, 0, sizeof(*value));
    return UMI_STATUS_OK;
}
