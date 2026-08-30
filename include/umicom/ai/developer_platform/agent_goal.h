/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/agent_goal.h
 *
 * PURPOSE:
 *   Describe a governed AI developer goal without granting execution authority.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_AGENT_GOAL_H
#define UMICOM_AI_DEVELOPER_PLATFORM_AGENT_GOAL_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevAgentGoal {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevAgentGoal;

void umi_ai_dev_agent_goal_init(UmiAiDevAgentGoal *value);
UmiStatus umi_ai_dev_agent_goal_configure(UmiAiDevAgentGoal *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
UmiStatus umi_ai_dev_agent_goal_validate(const UmiAiDevAgentGoal *value);
uint32_t umi_ai_dev_agent_goal_evidence_score(const UmiAiDevAgentGoal *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
