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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiAiDevDeveloperSession {
    UmiAiDevState state;
    uint32_t successes;
    uint32_t failures;
    uint32_t pending;
    uint64_t revision;
} UmiAiDevDeveloperSession;

void umi_ai_dev_developer_session_init(UmiAiDevDeveloperSession *value);
UmiStatus umi_ai_dev_developer_session_record(UmiAiDevDeveloperSession *value, int success);
void umi_ai_dev_developer_session_set_pending(UmiAiDevDeveloperSession *value, uint32_t pending);
uint32_t umi_ai_dev_developer_session_health_score(const UmiAiDevDeveloperSession *value);
int umi_ai_dev_developer_session_ready(const UmiAiDevDeveloperSession *value);

#ifdef __cplusplus
}
#endif

#endif
