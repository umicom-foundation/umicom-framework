/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/document.h
 *
 * PURPOSE:
 *   Describe one source document that may be indexed or retrieved.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Documents use stable IDs and revisions so retrieval results can point back to an exact source.
 */

#ifndef INCLUDE_UMICOM_AI_DOCUMENT_H
#define INCLUDE_UMICOM_AI_DOCUMENT_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai document data shared with callers of this public contract.
 */
typedef struct UmiAiDocument {
    char document_id[UMI_AI_ID_CAPACITY];
    char title[UMI_AI_SMALL_TEXT_CAPACITY];
    char uri[UMI_AI_TEXT_CAPACITY];
    uint64_t revision;
} UmiAiDocument;

/**
 * Initialise ai document from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ai_document_init(UmiAiDocument *value);

#ifdef __cplusplus
}
#endif

#endif
