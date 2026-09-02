/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/knowledge/collection.c
 * PURPOSE: Initialise validated knowledge collection descriptors.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/knowledge/collection.h"

#include <stdio.h>
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static int copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) return 0;
    written = snprintf(destination, capacity, "%s", source);
    return written >= 0 && (size_t)written < capacity;
}

/*
 * Initialise knowledge collection from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_knowledge_collection_init(UmiKnowledgeCollection *collection,
                                        const char *collection_id,
                                        const char *display_name,
                                        const char *description)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (collection == NULL || collection_id == NULL ||
        collection_id[0] == '\0' || display_name == NULL ||
        display_name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(collection, 0, sizeof(*collection));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!copy_text(collection->collection_id,
                   sizeof(collection->collection_id), collection_id) ||
        !copy_text(collection->display_name,
                   sizeof(collection->display_name), display_name) ||
        !copy_text(collection->description,
                   sizeof(collection->description),
                   description != NULL ? description : "")) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    collection->revision = 1U;
    collection->enabled = 1;
    return UMI_STATUS_OK;
}
