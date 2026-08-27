/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/helix/evidence_context.h
 *
 * PURPOSE:
 *   Assemble bounded labelled evidence for AI reasoning without introducing new persistence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_AI_HELIX_EVIDENCE_CONTEXT_H
#define INCLUDE_UMICOM_AI_HELIX_EVIDENCE_CONTEXT_H

#include <stddef.h>
#include "umicom/ai/helix/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiHelixEvidenceContext {
    char text[UMI_AI_HELIX_CONTEXT_CAPACITY];
    size_t length;
    size_t item_count;
} UmiAiHelixEvidenceContext;

void umi_ai_helix_evidence_context_init(UmiAiHelixEvidenceContext *context);
UmiStatus umi_ai_helix_evidence_context_add(UmiAiHelixEvidenceContext *context,
                                            const char *label,
                                            const char *text);

#ifdef __cplusplus
}
#endif

#endif
