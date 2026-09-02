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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the ai dev agent plan data shared with callers of this public contract.
 */
typedef struct UmiAiDevAgentPlan { char ids[UMI_AI_DEV_MEDIUM_CAPACITY][UMI_AI_DEV_ID_CAPACITY]; size_t count; uint64_t revision; } UmiAiDevAgentPlan;
/**
 * Initialise ai dev agent plan from caller-provided values so later operations receive a
 * known state.
 */
void umi_ai_dev_agent_plan_init(UmiAiDevAgentPlan *collection);
/**
 * Add ai dev agent plan only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ai_dev_agent_plan_add(UmiAiDevAgentPlan *collection, const char *id);
/**
 * Remove ai dev agent plan while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ai_dev_agent_plan_remove(UmiAiDevAgentPlan *collection, const char *id);
/**
 * Provide the ai dev agent plan contains operation used by this module and its client
 * applications.
 */
int umi_ai_dev_agent_plan_contains(const UmiAiDevAgentPlan *collection, const char *id);
/**
 * Return the number of records represented by ai dev agent plan without changing their
 * state.
 */
size_t umi_ai_dev_agent_plan_count(const UmiAiDevAgentPlan *collection);

#ifdef __cplusplus
}
#endif

#endif
