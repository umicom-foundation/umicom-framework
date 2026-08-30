/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/source.h
 *
 * PURPOSE:
 *   Describe a retrievable source and its human-readable attribution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Source attribution is separated from generated text so UIs can show where retrieved evidence came from.
 */

#ifndef INCLUDE_UMICOM_AI_SOURCE_H
#define INCLUDE_UMICOM_AI_SOURCE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiSource {
    char source_id[UMI_AI_ID_CAPACITY];
    char title[UMI_AI_SMALL_TEXT_CAPACITY];
    char uri[UMI_AI_TEXT_CAPACITY];
} UmiAiSource;

UmiStatus umi_ai_source_init(UmiAiSource *value);

#ifdef __cplusplus
}
#endif

#endif
