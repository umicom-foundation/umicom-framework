/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/context_redaction.h
 *
 * PURPOSE:
 *   Decide whether sensitive context must be excluded or redacted before provider use.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_REDACTION_H
#define UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_REDACTION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev context redaction data shared with callers of this public contract.
 */
typedef struct UmiAiDevContextRedaction {
    uint64_t allowed_flags;
    uint64_t denied_flags;
    UmiAiDevRisk maximum_risk;
    UmiAiDevLocality locality;
    int require_approval;
    int enabled;
} UmiAiDevContextRedaction;

/**
 * Initialise ai dev context redaction from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_context_redaction_init(UmiAiDevContextRedaction *policy);
/**
 * Provide the ai dev context redaction configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_dev_context_redaction_configure(UmiAiDevContextRedaction *policy, uint64_t allowed_flags, uint64_t denied_flags, UmiAiDevRisk maximum_risk, UmiAiDevLocality locality, int require_approval);
/**
 * Provide the ai dev context redaction allows operation used by this module and its client
 * applications.
 */
int umi_ai_dev_context_redaction_allows(const UmiAiDevContextRedaction *policy, uint64_t requested_flags, UmiAiDevRisk risk, UmiAiDevLocality locality);
/**
 * Provide the ai dev context redaction approval required operation used by this module and
 * its client applications.
 */
int umi_ai_dev_context_redaction_approval_required(const UmiAiDevContextRedaction *policy, UmiAiDevRisk risk);

#ifdef __cplusplus
}
#endif

#endif
