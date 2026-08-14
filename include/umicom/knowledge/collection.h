/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/knowledge/collection.h
 * PURPOSE: Describe one independently searchable knowledge collection.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: A collection is a named search boundary, similar to a shelf. */
#ifndef UMICOM_KNOWLEDGE_COLLECTION_H
#define UMICOM_KNOWLEDGE_COLLECTION_H

#include "umicom/knowledge/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiKnowledgeCollection {
    char collection_id[UMI_KNOWLEDGE_ID_CAPACITY];
    char display_name[UMI_KNOWLEDGE_NAME_CAPACITY];
    char description[UMI_KNOWLEDGE_TEXT_CAPACITY];
    uint64_t revision;
    int enabled;
} UmiKnowledgeCollection;

UmiStatus umi_knowledge_collection_init(UmiKnowledgeCollection *collection,
                                        const char *collection_id,
                                        const char *display_name,
                                        const char *description);

#ifdef __cplusplus
}
#endif
#endif
