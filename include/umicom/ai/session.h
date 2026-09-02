/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/session.h
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

#ifndef INCLUDE_UMICOM_AI_SESSION_H
#define INCLUDE_UMICOM_AI_SESSION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai session data shared with callers of this public contract.
 */
typedef struct UmiAiSession {
    char session_id[UMI_AI_ID_CAPACITY];
    char provider_id[UMI_AI_ID_CAPACITY];
    char model_id[UMI_AI_ID_CAPACITY];
    uint64_t created_at_ns;
} UmiAiSession;

/**
 * Initialise ai session from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ai_session_init(UmiAiSession *value);

#ifdef __cplusplus
}
#endif

#endif
