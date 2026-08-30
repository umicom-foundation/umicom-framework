/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/model_descriptor.h
 *
 * PURPOSE:
 *   Describe a model endpoint without coupling applications to provider-specific configuration.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_MODEL_DESCRIPTOR_H
#define UMICOM_AI_DEVELOPER_PLATFORM_MODEL_DESCRIPTOR_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevModelDescriptor {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevModelDescriptor;

void umi_ai_dev_model_descriptor_init(UmiAiDevModelDescriptor *value);
UmiStatus umi_ai_dev_model_descriptor_configure(UmiAiDevModelDescriptor *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
UmiStatus umi_ai_dev_model_descriptor_validate(const UmiAiDevModelDescriptor *value);
uint32_t umi_ai_dev_model_descriptor_evidence_score(const UmiAiDevModelDescriptor *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
