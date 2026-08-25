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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiAiDevDependencyContext {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevDependencyContext;

void umi_ai_dev_dependency_context_init(UmiAiDevDependencyContext *value);
UmiStatus umi_ai_dev_dependency_context_configure(UmiAiDevDependencyContext *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
UmiStatus umi_ai_dev_dependency_context_validate(const UmiAiDevDependencyContext *value);
uint32_t umi_ai_dev_dependency_context_evidence_score(const UmiAiDevDependencyContext *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
