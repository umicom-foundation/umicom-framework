/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/agent_approval_gate.h
 *
 * PURPOSE:
 *   Decide when a planned agent operation requires explicit user approval.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_AGENT_APPROVAL_GATE_H
#define UMICOM_AI_DEVELOPER_PLATFORM_AGENT_APPROVAL_GATE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevAgentApprovalGate {
    uint64_t allowed_flags;
    uint64_t denied_flags;
    UmiAiDevRisk maximum_risk;
    UmiAiDevLocality locality;
    int require_approval;
    int enabled;
} UmiAiDevAgentApprovalGate;

void umi_ai_dev_agent_approval_gate_init(UmiAiDevAgentApprovalGate *policy);
UmiStatus umi_ai_dev_agent_approval_gate_configure(UmiAiDevAgentApprovalGate *policy, uint64_t allowed_flags, uint64_t denied_flags, UmiAiDevRisk maximum_risk, UmiAiDevLocality locality, int require_approval);
int umi_ai_dev_agent_approval_gate_allows(const UmiAiDevAgentApprovalGate *policy, uint64_t requested_flags, UmiAiDevRisk risk, UmiAiDevLocality locality);
int umi_ai_dev_agent_approval_gate_approval_required(const UmiAiDevAgentApprovalGate *policy, UmiAiDevRisk risk);

#ifdef __cplusplus
}
#endif

#endif
