/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/repository_document.h
 *
 * PURPOSE:
 *   Describe an indexed repository document and its revision evidence.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_REPOSITORY_DOCUMENT_H
#define UMICOM_AI_DEVELOPER_PLATFORM_REPOSITORY_DOCUMENT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevRepositoryDocument {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevRepositoryDocument;

void umi_ai_dev_repository_document_init(UmiAiDevRepositoryDocument *value);
UmiStatus umi_ai_dev_repository_document_configure(UmiAiDevRepositoryDocument *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
UmiStatus umi_ai_dev_repository_document_validate(const UmiAiDevRepositoryDocument *value);
uint32_t umi_ai_dev_repository_document_evidence_score(const UmiAiDevRepositoryDocument *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
