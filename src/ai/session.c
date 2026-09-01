/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/session.c
 *
 * PURPOSE:
 *   Record the selected provider and model for one AI interaction session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A session records provider/model selection explicitly so changing a model does not silently alter an existing conversation.
 */

#include "umicom/ai/session.h"
#include <stddef.h>

#include <string.h>

UmiStatus umi_ai_session_init(UmiAiSession *value)
{
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(value, 0, sizeof(*value));
    return UMI_STATUS_OK;
}
