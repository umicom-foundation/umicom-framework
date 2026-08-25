/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/agent_plan.h
 *
 * PURPOSE:
 *   Collect ordered plan-step identities for governed execution.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_AGENT_PLAN_H
#define UMICOM_AI_DEVELOPER_PLATFORM_AGENT_PLAN_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevAgentPlan { char ids[UMI_AI_DEV_MEDIUM_CAPACITY][UMI_AI_DEV_ID_CAPACITY]; size_t count; uint64_t revision; } UmiAiDevAgentPlan;
void umi_ai_dev_agent_plan_init(UmiAiDevAgentPlan *collection);
UmiStatus umi_ai_dev_agent_plan_add(UmiAiDevAgentPlan *collection, const char *id);
UmiStatus umi_ai_dev_agent_plan_remove(UmiAiDevAgentPlan *collection, const char *id);
int umi_ai_dev_agent_plan_contains(const UmiAiDevAgentPlan *collection, const char *id);
size_t umi_ai_dev_agent_plan_count(const UmiAiDevAgentPlan *collection);

#ifdef __cplusplus
}
#endif

#endif
