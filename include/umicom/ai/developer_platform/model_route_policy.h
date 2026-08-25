/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/model_route_policy.h
 *
 * PURPOSE:
 *   Apply locality, privacy and capability constraints to model routing.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_MODEL_ROUTE_POLICY_H
#define UMICOM_AI_DEVELOPER_PLATFORM_MODEL_ROUTE_POLICY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevModelRoutePolicy {
    uint64_t allowed_flags;
    uint64_t denied_flags;
    UmiAiDevRisk maximum_risk;
    UmiAiDevLocality locality;
    int require_approval;
    int enabled;
} UmiAiDevModelRoutePolicy;

void umi_ai_dev_model_route_policy_init(UmiAiDevModelRoutePolicy *policy);
UmiStatus umi_ai_dev_model_route_policy_configure(UmiAiDevModelRoutePolicy *policy, uint64_t allowed_flags, uint64_t denied_flags, UmiAiDevRisk maximum_risk, UmiAiDevLocality locality, int require_approval);
int umi_ai_dev_model_route_policy_allows(const UmiAiDevModelRoutePolicy *policy, uint64_t requested_flags, UmiAiDevRisk risk, UmiAiDevLocality locality);
int umi_ai_dev_model_route_policy_approval_required(const UmiAiDevModelRoutePolicy *policy, UmiAiDevRisk risk);

#ifdef __cplusplus
}
#endif

#endif
