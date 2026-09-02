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

/**
 * Represent the ai dev memory retention data shared with callers of this public contract.
 */
typedef struct UmiAiDevMemoryRetention {
    uint64_t allowed_flags;
    uint64_t denied_flags;
    UmiAiDevRisk maximum_risk;
    UmiAiDevLocality locality;
    int require_approval;
    int enabled;
} UmiAiDevMemoryRetention;

/**
 * Initialise ai dev memory retention from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_memory_retention_init(UmiAiDevMemoryRetention *policy);
/**
 * Provide the ai dev memory retention configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_dev_memory_retention_configure(UmiAiDevMemoryRetention *policy, uint64_t allowed_flags, uint64_t denied_flags, UmiAiDevRisk maximum_risk, UmiAiDevLocality locality, int require_approval);
/**
 * Provide the ai dev memory retention allows operation used by this module and its client
 * applications.
 */
int umi_ai_dev_memory_retention_allows(const UmiAiDevMemoryRetention *policy, uint64_t requested_flags, UmiAiDevRisk risk, UmiAiDevLocality locality);
/**
 * Provide the ai dev memory retention approval required operation used by this module and
 * its client applications.
 */
int umi_ai_dev_memory_retention_approval_required(const UmiAiDevMemoryRetention *policy, UmiAiDevRisk risk);

#ifdef __cplusplus
}
#endif

#endif
