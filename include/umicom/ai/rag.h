/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/rag.h
 *
 * PURPOSE:
 *   Compose retrieval evidence into a bounded prompt context.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * RAG composition is separated from retrieval so different indexes can feed the same prompt-building policy.
 */

#ifndef INCLUDE_UMICOM_AI_RAG_H
#define INCLUDE_UMICOM_AI_RAG_H

#include <stddef.h>
#include "umicom/ai/retrieval.h"
#include "umicom/ai/prompt.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_rag_append_results(UmiAiPrompt *prompt,
                                    const UmiAiRetrievalResult *results,
                                    size_t count);

#ifdef __cplusplus
}
#endif

#endif
