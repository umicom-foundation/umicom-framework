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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiAiDevAgentTask {
    UmiAiDevState state;
    uint32_t successes;
    uint32_t failures;
    uint32_t pending;
    uint64_t revision;
} UmiAiDevAgentTask;

void umi_ai_dev_agent_task_init(UmiAiDevAgentTask *value);
UmiStatus umi_ai_dev_agent_task_record(UmiAiDevAgentTask *value, int success);
void umi_ai_dev_agent_task_set_pending(UmiAiDevAgentTask *value, uint32_t pending);
uint32_t umi_ai_dev_agent_task_health_score(const UmiAiDevAgentTask *value);
int umi_ai_dev_agent_task_ready(const UmiAiDevAgentTask *value);

#ifdef __cplusplus
}
#endif

#endif
