/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/model_runtime_binding.h
 *
 * PURPOSE:
 *   Bind a model to a Framework AI runtime without moving runtime ownership into applications.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_MODEL_RUNTIME_BINDING_H
#define UMICOM_AI_DEVELOPER_PLATFORM_MODEL_RUNTIME_BINDING_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai dev model runtime binding data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDevModelRuntimeBinding {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevModelRuntimeBinding;

/**
 * Initialise ai dev model runtime binding from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_model_runtime_binding_init(UmiAiDevModelRuntimeBinding *value);
/**
 * Provide the ai dev model runtime binding configure operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_dev_model_runtime_binding_configure(UmiAiDevModelRuntimeBinding *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
/**
 * Check that ai dev model runtime binding satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_ai_dev_model_runtime_binding_validate(const UmiAiDevModelRuntimeBinding *value);
/**
 * Provide the ai dev model runtime binding evidence score operation used by this module
 * and its client applications.
 */
uint32_t umi_ai_dev_model_runtime_binding_evidence_score(const UmiAiDevModelRuntimeBinding *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
