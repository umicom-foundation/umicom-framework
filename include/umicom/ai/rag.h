/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/rag.h
 *
 * PURPOSE:
 *   Compose retrieval evidence into a bounded prompt context.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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

/**
 * Provide the ai rag append results operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_rag_append_results(UmiAiPrompt *prompt,
                                    const UmiAiRetrievalResult *results,
                                    size_t count);

#ifdef __cplusplus
}
#endif

#endif
