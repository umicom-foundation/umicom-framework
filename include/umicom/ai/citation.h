/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/citation.h
 *
 * PURPOSE:
 *   Describe one citation from generated or retrieved content back to a source.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A citation keeps source identity and score explicit rather than hiding provenance inside generated prose.
 */

#ifndef INCLUDE_UMICOM_AI_CITATION_H
#define INCLUDE_UMICOM_AI_CITATION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiCitation {
    char source_id[UMI_AI_ID_CAPACITY];
    char label[UMI_AI_SMALL_TEXT_CAPACITY];
    double score;
    size_t start_offset;
    size_t end_offset;
} UmiAiCitation;

UmiStatus umi_ai_citation_init(UmiAiCitation *value);

#ifdef __cplusplus
}
#endif

#endif
