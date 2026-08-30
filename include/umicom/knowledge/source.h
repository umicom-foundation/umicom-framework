/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/knowledge/source.h
 * PURPOSE: Initialise sources and decide whether incremental refresh is needed.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Content hashes let unchanged sources skip expensive re-indexing. */
#ifndef UMICOM_KNOWLEDGE_SOURCE_H
#define UMICOM_KNOWLEDGE_SOURCE_H

#include "umicom/knowledge/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_knowledge_source_init(UmiKnowledgeSource *source,
                                    const char *source_id,
                                    const char *collection_id,
                                    const char *title,
                                    const char *uri,
                                    UmiKnowledgeSourceKind kind);
UmiKnowledgeRefreshDecision umi_knowledge_source_refresh_decision(
    const UmiKnowledgeSource *current,
    const UmiKnowledgeSource *candidate);

#ifdef __cplusplus
}
#endif
#endif
