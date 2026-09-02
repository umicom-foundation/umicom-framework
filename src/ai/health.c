/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/health.c
 *
 * PURPOSE:
 *   Record the current operational health of an AI provider.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Provider health lets Studio distinguish model errors from application errors and present a clear operational status.
 */

#include "umicom/ai/health.h"
#include <stddef.h>

#include <string.h>

/*
 * Initialise ai health from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ai_health_init(UmiAiHealth *value)
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
