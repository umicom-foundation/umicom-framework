/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/symbol_index.h
 *
 * PURPOSE:
 *   Define a scalable, provider-neutral workspace symbol index shared by
 *   native parsers, language servers, compiler frontends and future AI tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_SYMBOL_INDEX_H
#define UMICOM_EDITOR_SYMBOL_INDEX_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/source_location.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_SYMBOL_INDEX_API_VERSION 1U
#define UMI_EDITOR_SYMBOL_INDEX_DEFAULT_CAPACITY 512U
#define UMI_EDITOR_SYMBOL_ID_CAPACITY 128U
#define UMI_EDITOR_SYMBOL_NAME_CAPACITY 256U
#define UMI_EDITOR_SYMBOL_QUALIFIED_NAME_CAPACITY 512U
#define UMI_EDITOR_SYMBOL_SIGNATURE_CAPACITY 512U
#define UMI_EDITOR_SYMBOL_DETAIL_CAPACITY 512U
#define UMI_EDITOR_SYMBOL_PROVIDER_CAPACITY 128U
#define UMI_EDITOR_SYMBOL_WORKSPACE_CAPACITY 128U
#define UMI_EDITOR_SYMBOL_DOCUMENT_CAPACITY 128U
#define UMI_EDITOR_SYMBOL_LANGUAGE_CAPACITY 64U

/**
 * List the named editor indexed symbol kind values accepted by this public contract.
 */
typedef enum UmiEditorIndexedSymbolKind {
    UMI_EDITOR_INDEXED_SYMBOL_UNKNOWN = 0,
    UMI_EDITOR_INDEXED_SYMBOL_FILE = 1,
    UMI_EDITOR_INDEXED_SYMBOL_MODULE = 2,
    UMI_EDITOR_INDEXED_SYMBOL_NAMESPACE = 3,
    UMI_EDITOR_INDEXED_SYMBOL_PACKAGE = 4,
    UMI_EDITOR_INDEXED_SYMBOL_CLASS = 5,
    UMI_EDITOR_INDEXED_SYMBOL_METHOD = 6,
    UMI_EDITOR_INDEXED_SYMBOL_PROPERTY = 7,
    UMI_EDITOR_INDEXED_SYMBOL_FIELD = 8,
    UMI_EDITOR_INDEXED_SYMBOL_CONSTRUCTOR = 9,
    UMI_EDITOR_INDEXED_SYMBOL_ENUM = 10,
    UMI_EDITOR_INDEXED_SYMBOL_INTERFACE = 11,
    UMI_EDITOR_INDEXED_SYMBOL_FUNCTION = 12,
    UMI_EDITOR_INDEXED_SYMBOL_VARIABLE = 13,
    UMI_EDITOR_INDEXED_SYMBOL_CONSTANT = 14,
    UMI_EDITOR_INDEXED_SYMBOL_STRING = 15,
    UMI_EDITOR_INDEXED_SYMBOL_NUMBER = 16,
    UMI_EDITOR_INDEXED_SYMBOL_BOOLEAN = 17,
    UMI_EDITOR_INDEXED_SYMBOL_ARRAY = 18,
    UMI_EDITOR_INDEXED_SYMBOL_OBJECT = 19,
    UMI_EDITOR_INDEXED_SYMBOL_KEY = 20,
    UMI_EDITOR_INDEXED_SYMBOL_NULL = 21,
    UMI_EDITOR_INDEXED_SYMBOL_ENUM_MEMBER = 22,
    UMI_EDITOR_INDEXED_SYMBOL_STRUCTURE = 23,
    UMI_EDITOR_INDEXED_SYMBOL_EVENT = 24,
    UMI_EDITOR_INDEXED_SYMBOL_OPERATOR = 25,
    UMI_EDITOR_INDEXED_SYMBOL_TYPE_PARAMETER = 26,
    UMI_EDITOR_INDEXED_SYMBOL_MACRO = 27,
    UMI_EDITOR_INDEXED_SYMBOL_LABEL = 28
} UmiEditorIndexedSymbolKind;

/**
 * List the named editor indexed symbol flags values accepted by this public contract.
 */
typedef enum UmiEditorIndexedSymbolFlags {
    UMI_EDITOR_INDEXED_SYMBOL_FLAG_NONE = 0,
    UMI_EDITOR_INDEXED_SYMBOL_FLAG_DEFINITION = 1U << 0,
    UMI_EDITOR_INDEXED_SYMBOL_FLAG_DECLARATION = 1U << 1,
    UMI_EDITOR_INDEXED_SYMBOL_FLAG_DEPRECATED = 1U << 2,
    UMI_EDITOR_INDEXED_SYMBOL_FLAG_EXTERNAL = 1U << 3,
    UMI_EDITOR_INDEXED_SYMBOL_FLAG_GENERATED = 1U << 4,
    UMI_EDITOR_INDEXED_SYMBOL_FLAG_READ_ONLY = 1U << 5,
    UMI_EDITOR_INDEXED_SYMBOL_FLAG_TEST = 1U << 6
} UmiEditorIndexedSymbolFlags;

/**
 * Represent the editor indexed symbol data shared with callers of this public contract.
 */
typedef struct UmiEditorIndexedSymbol {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_SYMBOL_ID_CAPACITY];
    char parent_id[UMI_EDITOR_SYMBOL_ID_CAPACITY];
    char provider_id[UMI_EDITOR_SYMBOL_PROVIDER_CAPACITY];
    char workspace_id[UMI_EDITOR_SYMBOL_WORKSPACE_CAPACITY];
    char document_id[UMI_EDITOR_SYMBOL_DOCUMENT_CAPACITY];
    char language_id[UMI_EDITOR_SYMBOL_LANGUAGE_CAPACITY];
    char name[UMI_EDITOR_SYMBOL_NAME_CAPACITY];
    char qualified_name[UMI_EDITOR_SYMBOL_QUALIFIED_NAME_CAPACITY];
    char container_name[UMI_EDITOR_SYMBOL_NAME_CAPACITY];
    char signature[UMI_EDITOR_SYMBOL_SIGNATURE_CAPACITY];
    char detail[UMI_EDITOR_SYMBOL_DETAIL_CAPACITY];
    UmiEditorIndexedSymbolKind kind;
    uint32_t flags;
    UmiEditorSourceLocation location;
    UmiEditorSourceLocation selection_location;
    uint64_t sequence;
    uint64_t revision;
} UmiEditorIndexedSymbol;

/**
 * Represent the editor symbol query data shared with callers of this public contract.
 */
typedef struct UmiEditorSymbolQuery {
    uint32_t struct_size;
    uint32_t api_version;
    const char *text;
    const char *workspace_id;
    const char *document_id;
    const char *language_id;
    UmiEditorIndexedSymbolKind kind;
    uint32_t required_flags;
    size_t maximum_results;
    int case_sensitive;
    int include_external;
} UmiEditorSymbolQuery;

/**
 * Represent the editor symbol match data shared with callers of this public contract.
 */
typedef struct UmiEditorSymbolMatch {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorIndexedSymbol symbol;
    int32_t score;
    size_t matched_character_count;
} UmiEditorSymbolMatch;

/**
 * Represent the editor symbol index data shared with callers of this public contract.
 */
typedef struct UmiEditorSymbolIndex UmiEditorSymbolIndex;

/**
 * Initialise editor symbol index from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_editor_symbol_index_create(size_t initial_capacity,
                                          UmiEditorSymbolIndex **out_index);
/**
 * Release or reset state held by editor symbol index so the same storage can be reused
 * safely.
 */
void umi_editor_symbol_index_destroy(UmiEditorSymbolIndex *index);
/**
 * Provide the editor symbol index upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_symbol_index_upsert(UmiEditorSymbolIndex *index,
                                          const UmiEditorIndexedSymbol *symbol);
/**
 * Remove editor symbol index while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_symbol_index_remove(UmiEditorSymbolIndex *index,
                                          const char *symbol_id);
/**
 * Provide the editor symbol index remove document operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_symbol_index_remove_document(UmiEditorSymbolIndex *index,
                                                   const char *document_id,
                                                   size_t *out_removed_count);
/**
 * Find editor symbol index while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_editor_symbol_index_find(const UmiEditorSymbolIndex *index,
                                        const char *symbol_id,
                                        UmiEditorIndexedSymbol *out_symbol);
/**
 * Find editor symbol index while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_editor_symbol_index_at(const UmiEditorSymbolIndex *index,
                                      size_t position,
                                      UmiEditorIndexedSymbol *out_symbol);
/**
 * Provide the editor symbol index find enclosing operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_symbol_index_find_enclosing(
    const UmiEditorSymbolIndex *index,
    const UmiEditorSourceLocation *location,
    UmiEditorIndexedSymbol *out_symbol);
/**
 * Provide the editor symbol index search operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_symbol_index_search(
    const UmiEditorSymbolIndex *index,
    const UmiEditorSymbolQuery *query,
    UmiEditorSymbolMatch *out_matches,
    size_t match_capacity,
    size_t *out_match_count);
/**
 * Provide the editor symbol index children operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_symbol_index_children(
    const UmiEditorSymbolIndex *index,
    const char *parent_id,
    const char *document_id,
    UmiEditorIndexedSymbol *out_symbols,
    size_t symbol_capacity,
    size_t *out_symbol_count);
/**
 * Return the number of records represented by editor symbol index without changing their
 * state.
 */
size_t umi_editor_symbol_index_count(const UmiEditorSymbolIndex *index);
/**
 * Provide the editor symbol index revision operation used by this module and its client
 * applications.
 */
uint64_t umi_editor_symbol_index_revision(const UmiEditorSymbolIndex *index);
/**
 * Provide the editor indexed symbol kind name operation used by this module and its client
 * applications.
 */
const char *umi_editor_indexed_symbol_kind_name(UmiEditorIndexedSymbolKind kind);
/**
 * Provide the editor indexed symbol kind from name operation used by this module and its
 * client applications.
 */
UmiEditorIndexedSymbolKind umi_editor_indexed_symbol_kind_from_name(
    const char *name);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_SYMBOL_INDEX_H */
