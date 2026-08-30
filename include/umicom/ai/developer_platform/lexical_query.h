/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/lexical_query.h
 *
 * PURPOSE:
 *   Score lexical retrieval candidates deterministically.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_LEXICAL_QUERY_H
#define UMICOM_AI_DEVELOPER_PLATFORM_LEXICAL_QUERY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevLexicalQueryScore { uint32_t relevance; uint32_t authority; uint32_t freshness; uint32_t penalty; } UmiAiDevLexicalQueryScore;
void umi_ai_dev_lexical_query_init(UmiAiDevLexicalQueryScore *score);
uint32_t umi_ai_dev_lexical_query_compute(const UmiAiDevLexicalQueryScore *score);
int umi_ai_dev_lexical_query_prefer(const UmiAiDevLexicalQueryScore *left, const UmiAiDevLexicalQueryScore *right);
uint32_t umi_ai_dev_lexical_query_weighted(uint32_t primary, uint32_t secondary, uint32_t primary_weight);

#ifdef __cplusplus
}
#endif

#endif
