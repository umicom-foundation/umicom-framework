/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/inline_session.h
 *
 * PURPOSE:
 *   Track the lifecycle of an inline assistance session.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_INLINE_SESSION_H
#define UMICOM_AI_DEVELOPER_PLATFORM_INLINE_SESSION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev inline session data shared with callers of this public contract.
 */
typedef struct UmiAiDevInlineSession {
    UmiAiDevState state;
    uint32_t successes;
    uint32_t failures;
    uint32_t pending;
    uint64_t revision;
} UmiAiDevInlineSession;

/**
 * Initialise ai dev inline session from caller-provided values so later operations receive
 * a known state.
 */
void umi_ai_dev_inline_session_init(UmiAiDevInlineSession *value);
/**
 * Provide the ai dev inline session record operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_inline_session_record(UmiAiDevInlineSession *value, int success);
/**
 * Provide the ai dev inline session set pending operation used by this module and its
 * client applications.
 */
void umi_ai_dev_inline_session_set_pending(UmiAiDevInlineSession *value, uint32_t pending);
/**
 * Provide the ai dev inline session health score operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_inline_session_health_score(const UmiAiDevInlineSession *value);
/**
 * Provide the ai dev inline session ready operation used by this module and its client
 * applications.
 */
int umi_ai_dev_inline_session_ready(const UmiAiDevInlineSession *value);

#ifdef __cplusplus
}
#endif

#endif
