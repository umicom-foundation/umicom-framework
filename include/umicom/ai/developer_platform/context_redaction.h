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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiAiDevContextRedaction {
    uint64_t allowed_flags;
    uint64_t denied_flags;
    UmiAiDevRisk maximum_risk;
    UmiAiDevLocality locality;
    int require_approval;
    int enabled;
} UmiAiDevContextRedaction;

void umi_ai_dev_context_redaction_init(UmiAiDevContextRedaction *policy);
UmiStatus umi_ai_dev_context_redaction_configure(UmiAiDevContextRedaction *policy, uint64_t allowed_flags, uint64_t denied_flags, UmiAiDevRisk maximum_risk, UmiAiDevLocality locality, int require_approval);
int umi_ai_dev_context_redaction_allows(const UmiAiDevContextRedaction *policy, uint64_t requested_flags, UmiAiDevRisk risk, UmiAiDevLocality locality);
int umi_ai_dev_context_redaction_approval_required(const UmiAiDevContextRedaction *policy, UmiAiDevRisk risk);

#ifdef __cplusplus
}
#endif

#endif
