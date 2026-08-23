/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/outline.c
 *
 * PURPOSE:
 *   Build an outline from indexed symbols belonging to one document.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/outline.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_developer_outline_build(
    const UmiDeveloperSymbolIndex *index,
    const char *document_uri,
    UmiDeveloperOutlineModel *out_outline)
{
    size_t position;

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

    for (position = 0U;
         position < umi_developer_symbol_index_count(index);
         ++position) {
        UmiDeveloperSymbol symbol;

        if (umi_developer_symbol_index_at(
                index, position, &symbol) != UMI_STATUS_OK) {
            continue;
        }

        if (strcmp(symbol.location.uri, document_uri) == 0) {
            if (out_outline->count >= UMI_DEVELOPER_OUTLINE_CAPACITY) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            out_outline->symbols[out_outline->count++] = symbol;
        }
    }

    out_outline->revision = 1U;
    return UMI_STATUS_OK;
}
