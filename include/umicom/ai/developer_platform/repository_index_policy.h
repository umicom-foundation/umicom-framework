/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/repository_index_policy.h
 *
 * PURPOSE:
 *   Decide which repository resources are eligible for AI indexing.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_REPOSITORY_INDEX_POLICY_H
#define UMICOM_AI_DEVELOPER_PLATFORM_REPOSITORY_INDEX_POLICY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevRepositoryIndexPolicy {
    uint64_t allowed_flags;
    uint64_t denied_flags;
    UmiAiDevRisk maximum_risk;
    UmiAiDevLocality locality;
    int require_approval;
    int enabled;
} UmiAiDevRepositoryIndexPolicy;

void umi_ai_dev_repository_index_policy_init(UmiAiDevRepositoryIndexPolicy *policy);
UmiStatus umi_ai_dev_repository_index_policy_configure(UmiAiDevRepositoryIndexPolicy *policy, uint64_t allowed_flags, uint64_t denied_flags, UmiAiDevRisk maximum_risk, UmiAiDevLocality locality, int require_approval);
int umi_ai_dev_repository_index_policy_allows(const UmiAiDevRepositoryIndexPolicy *policy, uint64_t requested_flags, UmiAiDevRisk risk, UmiAiDevLocality locality);
int umi_ai_dev_repository_index_policy_approval_required(const UmiAiDevRepositoryIndexPolicy *policy, UmiAiDevRisk risk);

#ifdef __cplusplus
}
#endif

#endif
