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

/* Provide the contains folded operation used by this module and its client applications. */
static int contains_folded(const char *text, const char *query)
{
    const char *start;

    /* Apply this branch only when its contract condition is satisfied. */
    if (query[0] == '\0') return 1;

    /* Visit each bounded item once so every record receives the same rule. */
    for (start = text; *start != '\0'; ++start) {
        const char *a = start;
        const char *b = query;

        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (*a != '\0' && *b != '\0' &&
               tolower((unsigned char)*a) ==
                   tolower((unsigned char)*b)) {
            ++a;
            ++b;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (*b == '\0') return 1;
    }

    return 0;
}

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(
    const UmiDeveloperSymbolIndex *index,
    const char *symbol_id)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || symbol_id == NULL) return (size_t)-1;

    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < index->count; ++position) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(index->symbols[position].symbol_id, symbol_id) == 0) {
            return position;
        }
    }

    return (size_t)-1;
}

/*
 * Initialise developer symbol index from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_developer_symbol_index_create(
    UmiDeveloperSymbolIndex **out_index)
{
    UmiDeveloperSymbolIndex *index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_index == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_index = NULL;

    index = (UmiDeveloperSymbolIndex *)calloc(1U, sizeof(*index));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    index->revision = 1U;
    *out_index = index;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by developer symbol index so the same storage can be reused
 * safely.
 */
void umi_developer_symbol_index_destroy(
    UmiDeveloperSymbolIndex *index)
{
    free(index);
}

/*
 * Provide the developer symbol index upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_symbol_index_upsert(
    UmiDeveloperSymbolIndex *index,
    const UmiDeveloperSymbol *symbol)
{
    size_t position;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || symbol == NULL ||
        symbol->symbol_id[0] == '\0' ||
        symbol->name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_productivity_location_validate(
        &symbol->location);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    position = find_index(index, symbol->symbol_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (position == (size_t)-1) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index->count >= UMI_DEVELOPER_SYMBOL_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        position = index->count++;
    }

    index->symbols[position] = *symbol;
    index->symbols[position].revision = ++index->revision;
    return UMI_STATUS_OK;
}

/*
 * Find developer symbol index while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_developer_symbol_index_find(
    const UmiDeveloperSymbolIndex *index,
    const char *symbol_id,
    UmiDeveloperSymbol *out_symbol)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || symbol_id == NULL || out_symbol == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    position = find_index(index, symbol_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    *out_symbol = index->symbols[position];
    return UMI_STATUS_OK;
}

/*
 * Find developer symbol index while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_developer_symbol_index_at(
    const UmiDeveloperSymbolIndex *index,
    size_t position,
    UmiDeveloperSymbol *out_symbol)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || out_symbol == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= index->count) return UMI_STATUS_NOT_FOUND;

    *out_symbol = index->symbols[position];
    return UMI_STATUS_OK;
}

/*
 * Provide the developer symbol index search operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_symbol_index_search(
    const UmiDeveloperSymbolIndex *index,
    const char *query,
    UmiDeveloperSymbol *out_symbols,
    size_t capacity,
    size_t *out_count)
{
    size_t position;
    size_t used = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || query == NULL ||
        out_symbols == NULL || out_count == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U;
         position < index->count && used < capacity;
         ++position) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (contains_folded(index->symbols[position].name, query) ||
            contains_folded(index->symbols[position].container, query)) {
            out_symbols[used++] = index->symbols[position];
        }
    }

    *out_count = used;
    return used > 0U ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

/*
 * Return the number of records represented by developer symbol index without changing
 * their state.
 */
size_t umi_developer_symbol_index_count(
    const UmiDeveloperSymbolIndex *index)
{
    return index != NULL ? index->count : 0U;
}
