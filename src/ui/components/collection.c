/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/components/collection.c
 *
 * PURPOSE:
 *   Implement one toolkit-neutral reusable UI component concern.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one responsibility small and explicit. Read the public
 * structure/function declarations first, then follow the implementation in
 * the matching source file.
 */
#include "umicom/ui/components/collection.h"

#include <string.h>

/*
 * Initialise ui component collection from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_component_collection_init(UmiUiComponentCollection *collection)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (collection != NULL) {
        (void)memset(collection, 0, sizeof(*collection));
    }
}

/*
 * Add ui component collection only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ui_component_collection_add(UmiUiComponentCollection *collection,
                                          const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (collection == NULL || id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (collection->count >= UMI_UI_COMPONENT_COLLECTION_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    const size_t length = strlen(id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= UMI_UI_COMPONENT_ID_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(collection->ids[collection->count], id, length + 1U);
    ++collection->count;
    return UMI_STATUS_OK;
}
