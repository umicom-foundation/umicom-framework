/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/developer_session.h
 *
 * PURPOSE:
 *   Aggregate AI developer session state, budgets, approvals and evidence readiness.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_DEVELOPER_SESSION_H
#define UMICOM_AI_DEVELOPER_PLATFORM_DEVELOPER_SESSION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev developer session data shared with callers of this public contract.
 */
typedef struct UmiAiDevDeveloperSession {
    UmiAiDevState state;
    uint32_t successes;
    uint32_t failures;
    uint32_t pending;
    uint64_t revision;
} UmiAiDevDeveloperSession;

/**
 * Initialise ai dev developer session from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_developer_session_init(UmiAiDevDeveloperSession *value);
/**
 * Provide the ai dev developer session record operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_developer_session_record(UmiAiDevDeveloperSession *value, int success);
/**
 * Provide the ai dev developer session set pending operation used by this module and its
 * client applications.
 */
void umi_ai_dev_developer_session_set_pending(UmiAiDevDeveloperSession *value, uint32_t pending);
/**
 * Provide the ai dev developer session health score operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_developer_session_health_score(const UmiAiDevDeveloperSession *value);
/**
 * Provide the ai dev developer session ready operation used by this module and its client
 * applications.
 */
int umi_ai_dev_developer_session_ready(const UmiAiDevDeveloperSession *value);

#ifdef __cplusplus
}
#endif

#endif
