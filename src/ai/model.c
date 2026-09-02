/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/model.c
 *
 * PURPOSE:
 *   Describe one AI model independently of the provider that hosts it.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A model record stores capabilities and limits, while provider-specific loading details remain private to adapters.
 */

#include "umicom/ai/model.h"
#include <stddef.h>

#include <string.h>

/*
 * Initialise ai model from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ai_model_init(UmiAiModel *value)
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
