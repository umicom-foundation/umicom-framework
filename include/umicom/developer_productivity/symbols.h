/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/symbols.h
 *
 * PURPOSE:
 *   Define a provider-neutral symbol/index result used by Go to Symbol,
 *   outlines, AI context and future LSP adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_SYMBOLS_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_SYMBOLS_H

#include "umicom/developer_productivity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_SYMBOL_CAPACITY 1024U

/**
 * List the named developer symbol kind values accepted by this public contract.
 */
typedef enum UmiDeveloperSymbolKind {
    UMI_DEVELOPER_SYMBOL_FILE = 1,
    UMI_DEVELOPER_SYMBOL_MODULE = 2,
    UMI_DEVELOPER_SYMBOL_NAMESPACE = 3,
    UMI_DEVELOPER_SYMBOL_TYPE = 4,
    UMI_DEVELOPER_SYMBOL_FUNCTION = 5,
    UMI_DEVELOPER_SYMBOL_METHOD = 6,
    UMI_DEVELOPER_SYMBOL_VARIABLE = 7,
    UMI_DEVELOPER_SYMBOL_FIELD = 8,
    UMI_DEVELOPER_SYMBOL_ENUM = 9,
    UMI_DEVELOPER_SYMBOL_MACRO = 10
} UmiDeveloperSymbolKind;

/**
 * Represent the developer symbol data shared with callers of this public contract.
 */
typedef struct UmiDeveloperSymbol {
    char symbol_id[UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY];
    char name[UMI_DEVELOPER_PRODUCTIVITY_NAME_CAPACITY];
    char container[UMI_DEVELOPER_PRODUCTIVITY_NAME_CAPACITY];
    char language_id[UMI_DEVELOPER_PRODUCTIVITY_ID_CAPACITY];
    UmiDeveloperSymbolKind kind;
    UmiDeveloperProductivityLocation location;
    uint64_t revision;
} UmiDeveloperSymbol;

/**
 * Represent the developer symbol index data shared with callers of this public contract.
 */
typedef struct UmiDeveloperSymbolIndex UmiDeveloperSymbolIndex;

/**
 * Initialise developer symbol index from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_developer_symbol_index_create(
    UmiDeveloperSymbolIndex **out_index);

/**
 * Release or reset state held by developer symbol index so the same storage can be reused
 * safely.
 */
void umi_developer_symbol_index_destroy(
    UmiDeveloperSymbolIndex *index);

/**
 * Provide the developer symbol index upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_symbol_index_upsert(
    UmiDeveloperSymbolIndex *index,
    const UmiDeveloperSymbol *symbol);

/**
 * Find developer symbol index while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_developer_symbol_index_find(
    const UmiDeveloperSymbolIndex *index,
    const char *symbol_id,
    UmiDeveloperSymbol *out_symbol);

/**
 * Find developer symbol index while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_developer_symbol_index_at(
    const UmiDeveloperSymbolIndex *index,
    size_t position,
    UmiDeveloperSymbol *out_symbol);

/**
 * Provide the developer symbol index search operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_symbol_index_search(
    const UmiDeveloperSymbolIndex *index,
    const char *query,
    UmiDeveloperSymbol *out_symbols,
    size_t capacity,
    size_t *out_count);

/**
 * Return the number of records represented by developer symbol index without changing
 * their state.
 */
size_t umi_developer_symbol_index_count(
    const UmiDeveloperSymbolIndex *index);

#ifdef __cplusplus
}
#endif

#endif
