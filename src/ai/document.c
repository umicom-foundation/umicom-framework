/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/document.c
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

#include "umicom/ai/document.h"
#include <stddef.h>

#include <string.h>

/*
 * Initialise ai document from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ai_document_init(UmiAiDocument *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(value, 0, sizeof(*value));
    return UMI_STATUS_OK;
}
