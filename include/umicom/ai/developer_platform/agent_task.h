/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/agent_task.h
 *
 * PURPOSE:
 *   Track one agent task lifecycle and bounded attempt count.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_AGENT_TASK_H
#define UMICOM_AI_DEVELOPER_PLATFORM_AGENT_TASK_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev agent task data shared with callers of this public contract.
 */
typedef struct UmiAiDevAgentTask {
    UmiAiDevState state;
    uint32_t successes;
    uint32_t failures;
    uint32_t pending;
    uint64_t revision;
} UmiAiDevAgentTask;

/**
 * Initialise ai dev agent task from caller-provided values so later operations receive a
 * known state.
 */
void umi_ai_dev_agent_task_init(UmiAiDevAgentTask *value);
/**
 * Provide the ai dev agent task record operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_agent_task_record(UmiAiDevAgentTask *value, int success);
/**
 * Provide the ai dev agent task set pending operation used by this module and its client
 * applications.
 */
void umi_ai_dev_agent_task_set_pending(UmiAiDevAgentTask *value, uint32_t pending);
/**
 * Provide the ai dev agent task health score operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_dev_agent_task_health_score(const UmiAiDevAgentTask *value);
/**
 * Provide the ai dev agent task ready operation used by this module and its client
 * applications.
 */
int umi_ai_dev_agent_task_ready(const UmiAiDevAgentTask *value);

#ifdef __cplusplus
}
#endif

#endif
