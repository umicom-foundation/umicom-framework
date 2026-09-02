/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/context_provenance.h
 *
 * PURPOSE:
 *   Track origin and revision evidence for a context item.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_PROVENANCE_H
#define UMICOM_AI_DEVELOPER_PLATFORM_CONTEXT_PROVENANCE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev context provenance data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDevContextProvenance {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevContextProvenance;

/**
 * Initialise ai dev context provenance from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_context_provenance_init(UmiAiDevContextProvenance *value);
/**
 * Provide the ai dev context provenance configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_dev_context_provenance_configure(UmiAiDevContextProvenance *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
/**
 * Check that ai dev context provenance satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_ai_dev_context_provenance_validate(const UmiAiDevContextProvenance *value);
/**
 * Provide the ai dev context provenance evidence score operation used by this module and
 * its client applications.
 */
uint32_t umi_ai_dev_context_provenance_evidence_score(const UmiAiDevContextProvenance *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
