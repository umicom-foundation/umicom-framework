/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/retrieval_result.h
 *
 * PURPOSE:
 *   Collect bounded retrieval result identities for context assembly.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_RETRIEVAL_RESULT_H
#define UMICOM_AI_DEVELOPER_PLATFORM_RETRIEVAL_RESULT_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevRetrievalResult { char ids[UMI_AI_DEV_MEDIUM_CAPACITY][UMI_AI_DEV_ID_CAPACITY]; size_t count; uint64_t revision; } UmiAiDevRetrievalResult;
void umi_ai_dev_retrieval_result_init(UmiAiDevRetrievalResult *collection);
UmiStatus umi_ai_dev_retrieval_result_add(UmiAiDevRetrievalResult *collection, const char *id);
UmiStatus umi_ai_dev_retrieval_result_remove(UmiAiDevRetrievalResult *collection, const char *id);
int umi_ai_dev_retrieval_result_contains(const UmiAiDevRetrievalResult *collection, const char *id);
size_t umi_ai_dev_retrieval_result_count(const UmiAiDevRetrievalResult *collection);

#ifdef __cplusplus
}
#endif

#endif
