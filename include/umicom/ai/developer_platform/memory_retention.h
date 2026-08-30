/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/memory_retention.h
 *
 * PURPOSE:
 *   Evaluate retention expiry without deleting memory implicitly.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_MEMORY_RETENTION_H
#define UMICOM_AI_DEVELOPER_PLATFORM_MEMORY_RETENTION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevMemoryRetention {
    uint64_t allowed_flags;
    uint64_t denied_flags;
    UmiAiDevRisk maximum_risk;
    UmiAiDevLocality locality;
    int require_approval;
    int enabled;
} UmiAiDevMemoryRetention;

void umi_ai_dev_memory_retention_init(UmiAiDevMemoryRetention *policy);
UmiStatus umi_ai_dev_memory_retention_configure(UmiAiDevMemoryRetention *policy, uint64_t allowed_flags, uint64_t denied_flags, UmiAiDevRisk maximum_risk, UmiAiDevLocality locality, int require_approval);
int umi_ai_dev_memory_retention_allows(const UmiAiDevMemoryRetention *policy, uint64_t requested_flags, UmiAiDevRisk risk, UmiAiDevLocality locality);
int umi_ai_dev_memory_retention_approval_required(const UmiAiDevMemoryRetention *policy, UmiAiDevRisk risk);

#ifdef __cplusplus
}
#endif

#endif
