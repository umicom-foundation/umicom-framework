/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/memory_privacy.h
 *
 * PURPOSE:
 *   Gate persistence/export of memory according to explicit privacy flags.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_MEMORY_PRIVACY_H
#define UMICOM_AI_DEVELOPER_PLATFORM_MEMORY_PRIVACY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev memory privacy data shared with callers of this public contract.
 */
typedef struct UmiAiDevMemoryPrivacy {
    uint64_t allowed_flags;
    uint64_t denied_flags;
    UmiAiDevRisk maximum_risk;
    UmiAiDevLocality locality;
    int require_approval;
    int enabled;
} UmiAiDevMemoryPrivacy;

/**
 * Initialise ai dev memory privacy from caller-provided values so later operations receive
 * a known state.
 */
void umi_ai_dev_memory_privacy_init(UmiAiDevMemoryPrivacy *policy);
/**
 * Provide the ai dev memory privacy configure operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_memory_privacy_configure(UmiAiDevMemoryPrivacy *policy, uint64_t allowed_flags, uint64_t denied_flags, UmiAiDevRisk maximum_risk, UmiAiDevLocality locality, int require_approval);
/**
 * Provide the ai dev memory privacy allows operation used by this module and its client
 * applications.
 */
int umi_ai_dev_memory_privacy_allows(const UmiAiDevMemoryPrivacy *policy, uint64_t requested_flags, UmiAiDevRisk risk, UmiAiDevLocality locality);
/**
 * Provide the ai dev memory privacy approval required operation used by this module and
 * its client applications.
 */
int umi_ai_dev_memory_privacy_approval_required(const UmiAiDevMemoryPrivacy *policy, UmiAiDevRisk risk);

#ifdef __cplusplus
}
#endif

#endif
