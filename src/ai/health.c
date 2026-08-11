/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/health.c
 *
 * PURPOSE:
 *   Record the current operational health of an AI provider.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Provider health lets Studio distinguish model errors from application errors and present a clear operational status.
 */

#include "umicom/ai/health.h"
#include <stddef.h>

#include <string.h>

UmiStatus umi_ai_health_init(UmiAiHealth *value)
{
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(value, 0, sizeof(*value));
    return UMI_STATUS_OK;
}
