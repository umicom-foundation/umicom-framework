/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/outline.c
 *
 * PURPOSE:
 *   Build an outline from indexed symbols belonging to one document.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/outline.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the developer outline build operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_outline_build(
    const UmiDeveloperSymbolIndex *index,
    const char *document_uri,
    UmiDeveloperOutlineModel *out_outline)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || document_uri == NULL ||
        out_outline == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_outline, 0, sizeof(*out_outline));
    (void)snprintf(
        out_outline->document_uri,
        sizeof(out_outline->document_uri),
        "%s",
        document_uri);

    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U;
         position < umi_developer_symbol_index_count(index);
         ++position) {
        UmiDeveloperSymbol symbol;

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_developer_symbol_index_at(
                index, position, &symbol) != UMI_STATUS_OK) {
            continue;
        }

        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(symbol.location.uri, document_uri) == 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (out_outline->count >= UMI_DEVELOPER_OUTLINE_CAPACITY) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_outline->symbols[out_outline->count++] = symbol;
        }
    }

    out_outline->revision = 1U;
    return UMI_STATUS_OK;
}
