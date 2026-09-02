/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/dependency_context.h
 *
 * PURPOSE:
 *   Describe dependency edges relevant to repository-aware AI context.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_DEPENDENCY_CONTEXT_H
#define UMICOM_AI_DEVELOPER_PLATFORM_DEPENDENCY_CONTEXT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev dependency context data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDevDependencyContext {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevDependencyContext;

/**
 * Initialise ai dev dependency context from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_dependency_context_init(UmiAiDevDependencyContext *value);
/**
 * Provide the ai dev dependency context configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_dev_dependency_context_configure(UmiAiDevDependencyContext *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
/**
 * Check that ai dev dependency context satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_ai_dev_dependency_context_validate(const UmiAiDevDependencyContext *value);
/**
 * Provide the ai dev dependency context evidence score operation used by this module and
 * its client applications.
 */
uint32_t umi_ai_dev_dependency_context_evidence_score(const UmiAiDevDependencyContext *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
