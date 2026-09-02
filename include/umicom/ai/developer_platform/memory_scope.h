/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/memory_scope.h
 *
 * PURPOSE:
 *   Gate memory visibility by workspace/project/application scope.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_MEMORY_SCOPE_H
#define UMICOM_AI_DEVELOPER_PLATFORM_MEMORY_SCOPE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev memory scope data shared with callers of this public contract.
 */
typedef struct UmiAiDevMemoryScope {
    uint64_t allowed_flags;
    uint64_t denied_flags;
    UmiAiDevRisk maximum_risk;
    UmiAiDevLocality locality;
    int require_approval;
    int enabled;
} UmiAiDevMemoryScope;

/**
 * Initialise ai dev memory scope from caller-provided values so later operations receive a
 * known state.
 */
void umi_ai_dev_memory_scope_init(UmiAiDevMemoryScope *policy);
/**
 * Provide the ai dev memory scope configure operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_memory_scope_configure(UmiAiDevMemoryScope *policy, uint64_t allowed_flags, uint64_t denied_flags, UmiAiDevRisk maximum_risk, UmiAiDevLocality locality, int require_approval);
/**
 * Provide the ai dev memory scope allows operation used by this module and its client
 * applications.
 */
int umi_ai_dev_memory_scope_allows(const UmiAiDevMemoryScope *policy, uint64_t requested_flags, UmiAiDevRisk risk, UmiAiDevLocality locality);
/**
 * Provide the ai dev memory scope approval required operation used by this module and its
 * client applications.
 */
int umi_ai_dev_memory_scope_approval_required(const UmiAiDevMemoryScope *policy, UmiAiDevRisk risk);

#ifdef __cplusplus
}
#endif

#endif
