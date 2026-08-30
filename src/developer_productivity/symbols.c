/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_productivity/symbols.c
 *
 * PURPOSE:
 *   Implement a bounded provider-neutral symbol index.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_productivity/symbols.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct UmiDeveloperSymbolIndex {
    UmiDeveloperSymbol symbols[UMI_DEVELOPER_SYMBOL_CAPACITY];
    size_t count;
    uint64_t revision;
};

static int contains_folded(const char *text, const char *query)
{
    const char *start;

    if (query[0] == '\0') return 1;

    for (start = text; *start != '\0'; ++start) {
        const char *a = start;
        const char *b = query;

        while (*a != '\0' && *b != '\0' &&
               tolower((unsigned char)*a) ==
                   tolower((unsigned char)*b)) {
            ++a;
            ++b;
        }

        if (*b == '\0') return 1;
    }

    return 0;
}

static size_t find_index(
    const UmiDeveloperSymbolIndex *index,
    const char *symbol_id)
{
    size_t position;

    if (index == NULL || symbol_id == NULL) return (size_t)-1;

    for (position = 0U; position < index->count; ++position) {
        if (strcmp(index->symbols[position].symbol_id, symbol_id) == 0) {
            return position;
        }
    }

    return (size_t)-1;
}

UmiStatus umi_developer_symbol_index_create(
    UmiDeveloperSymbolIndex **out_index)
{
    UmiDeveloperSymbolIndex *index;

    if (out_index == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_index = NULL;

    index = (UmiDeveloperSymbolIndex *)calloc(1U, sizeof(*index));
    if (index == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    index->revision = 1U;
    *out_index = index;
    return UMI_STATUS_OK;
}

void umi_developer_symbol_index_destroy(
    UmiDeveloperSymbolIndex *index)
{
    free(index);
}

UmiStatus umi_developer_symbol_index_upsert(
    UmiDeveloperSymbolIndex *index,
    const UmiDeveloperSymbol *symbol)
{
    size_t position;
    UmiStatus status;

    if (index == NULL || symbol == NULL ||
        symbol->symbol_id[0] == '\0' ||
        symbol->name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_productivity_location_validate(
        &symbol->location);
    if (status != UMI_STATUS_OK) return status;

    position = find_index(index, symbol->symbol_id);
    if (position == (size_t)-1) {
        if (index->count >= UMI_DEVELOPER_SYMBOL_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        position = index->count++;
    }

    index->symbols[position] = *symbol;
    index->symbols[position].revision = ++index->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_symbol_index_find(
    const UmiDeveloperSymbolIndex *index,
    const char *symbol_id,
    UmiDeveloperSymbol *out_symbol)
{
    size_t position;

    if (index == NULL || symbol_id == NULL || out_symbol == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    position = find_index(index, symbol_id);
    if (position == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    *out_symbol = index->symbols[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_symbol_index_at(
    const UmiDeveloperSymbolIndex *index,
    size_t position,
    UmiDeveloperSymbol *out_symbol)
{
    if (index == NULL || out_symbol == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (position >= index->count) return UMI_STATUS_NOT_FOUND;

    *out_symbol = index->symbols[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_symbol_index_search(
    const UmiDeveloperSymbolIndex *index,
    const char *query,
    UmiDeveloperSymbol *out_symbols,
    size_t capacity,
    size_t *out_count)
{
    size_t position;
    size_t used = 0U;

    if (index == NULL || query == NULL ||
        out_symbols == NULL || out_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (position = 0U;
         position < index->count && used < capacity;
         ++position) {
        if (contains_folded(index->symbols[position].name, query) ||
            contains_folded(index->symbols[position].container, query)) {
            out_symbols[used++] = index->symbols[position];
        }
    }

    *out_count = used;
    return used > 0U ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

size_t umi_developer_symbol_index_count(
    const UmiDeveloperSymbolIndex *index)
{
    return index != NULL ? index->count : 0U;
}
