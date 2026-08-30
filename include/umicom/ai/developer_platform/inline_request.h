/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/inline_request.h
 *
 * PURPOSE:
 *   Describe an inline code-assistance request with document revision evidence.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_INLINE_REQUEST_H
#define UMICOM_AI_DEVELOPER_PLATFORM_INLINE_REQUEST_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevInlineRequest {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevInlineRequest;

void umi_ai_dev_inline_request_init(UmiAiDevInlineRequest *value);
UmiStatus umi_ai_dev_inline_request_configure(UmiAiDevInlineRequest *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
UmiStatus umi_ai_dev_inline_request_validate(const UmiAiDevInlineRequest *value);
uint32_t umi_ai_dev_inline_request_evidence_score(const UmiAiDevInlineRequest *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
