/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/knowledge/collection.h
 * PURPOSE: Describe one independently searchable knowledge collection.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* A collection is a named search boundary, similar to a shelf. */
#ifndef UMICOM_KNOWLEDGE_COLLECTION_H
#define UMICOM_KNOWLEDGE_COLLECTION_H

#include "umicom/knowledge/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the knowledge collection data shared with callers of this public contract.
 */
typedef struct UmiKnowledgeCollection {
    char collection_id[UMI_KNOWLEDGE_ID_CAPACITY];
    char display_name[UMI_KNOWLEDGE_NAME_CAPACITY];
    char description[UMI_KNOWLEDGE_TEXT_CAPACITY];
    uint64_t revision;
    int enabled;
} UmiKnowledgeCollection;

/**
 * Initialise knowledge collection from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_knowledge_collection_init(UmiKnowledgeCollection *collection,
                                        const char *collection_id,
                                        const char *display_name,
                                        const char *description);

#ifdef __cplusplus
}
#endif
#endif
