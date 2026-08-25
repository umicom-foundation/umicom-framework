/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/agent_dependency.h
 *
 * PURPOSE:
 *   Decide whether an agent task dependency is satisfied.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_AGENT_DEPENDENCY_H
#define UMICOM_AI_DEVELOPER_PLATFORM_AGENT_DEPENDENCY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevAgentDependency {
    uint64_t allowed_flags;
    uint64_t denied_flags;
    UmiAiDevRisk maximum_risk;
    UmiAiDevLocality locality;
    int require_approval;
    int enabled;
} UmiAiDevAgentDependency;

void umi_ai_dev_agent_dependency_init(UmiAiDevAgentDependency *policy);
UmiStatus umi_ai_dev_agent_dependency_configure(UmiAiDevAgentDependency *policy, uint64_t allowed_flags, uint64_t denied_flags, UmiAiDevRisk maximum_risk, UmiAiDevLocality locality, int require_approval);
int umi_ai_dev_agent_dependency_allows(const UmiAiDevAgentDependency *policy, uint64_t requested_flags, UmiAiDevRisk risk, UmiAiDevLocality locality);
int umi_ai_dev_agent_dependency_approval_required(const UmiAiDevAgentDependency *policy, UmiAiDevRisk risk);

#ifdef __cplusplus
}
#endif

#endif
