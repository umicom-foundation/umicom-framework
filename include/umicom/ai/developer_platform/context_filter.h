/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/context_filter.h
 *
 * PURPOSE:
 *   Apply include/exclude and sensitivity gates to context candidates.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_FILTER_H
#define UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_FILTER_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev context filter data shared with callers of this public contract.
 */
typedef struct UmiAiDevContextFilter {
    uint64_t allowed_flags;
    uint64_t denied_flags;
    UmiAiDevRisk maximum_risk;
    UmiAiDevLocality locality;
    int require_approval;
    int enabled;
} UmiAiDevContextFilter;

/**
 * Initialise ai dev context filter from caller-provided values so later operations receive
 * a known state.
 */
void umi_ai_dev_context_filter_init(UmiAiDevContextFilter *policy);
/**
 * Provide the ai dev context filter configure operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_context_filter_configure(UmiAiDevContextFilter *policy, uint64_t allowed_flags, uint64_t denied_flags, UmiAiDevRisk maximum_risk, UmiAiDevLocality locality, int require_approval);
/**
 * Provide the ai dev context filter allows operation used by this module and its client
 * applications.
 */
int umi_ai_dev_context_filter_allows(const UmiAiDevContextFilter *policy, uint64_t requested_flags, UmiAiDevRisk risk, UmiAiDevLocality locality);
/**
 * Provide the ai dev context filter approval required operation used by this module and
 * its client applications.
 */
int umi_ai_dev_context_filter_approval_required(const UmiAiDevContextFilter *policy, UmiAiDevRisk risk);

#ifdef __cplusplus
}
#endif

#endif
