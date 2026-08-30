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

typedef struct UmiAiDevInlineSession {
    UmiAiDevState state;
    uint32_t successes;
    uint32_t failures;
    uint32_t pending;
    uint64_t revision;
} UmiAiDevInlineSession;

void umi_ai_dev_inline_session_init(UmiAiDevInlineSession *value);
UmiStatus umi_ai_dev_inline_session_record(UmiAiDevInlineSession *value, int success);
void umi_ai_dev_inline_session_set_pending(UmiAiDevInlineSession *value, uint32_t pending);
uint32_t umi_ai_dev_inline_session_health_score(const UmiAiDevInlineSession *value);
int umi_ai_dev_inline_session_ready(const UmiAiDevInlineSession *value);

#ifdef __cplusplus
}
#endif

#endif
