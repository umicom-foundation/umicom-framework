/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/symbol.h
 *
 * PURPOSE:
 *   Define document symbol and outline entries reusable with LSP or native parsers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This is a product-neutral C23 model. The registry owns snapshot copies by
 * value; callers own external resources and coordinate cross-thread mutation.
 */
#ifndef UMICOM_EDITOR_SYMBOL_H
#define UMICOM_EDITOR_SYMBOL_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_SYMBOL_CAPACITY 8192U
/**
 * Represent the editor symbol snapshot data shared with callers of this public contract.
 */
typedef struct UmiEditorSymbolSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char parent_id[128];
    char name[256];
    char kind[64];
    char detail[512];
    uint64_t line;
    uint64_t column;
    uint64_t end_line;
    uint64_t end_column;
    uint64_t revision;
} UmiEditorSymbolSnapshot;
/**
 * Represent the editor symbol registry data shared with callers of this public contract.
 */
typedef struct UmiEditorSymbolRegistry UmiEditorSymbolRegistry;
/**
 * Initialise editor symbol registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_symbol_registry_create(UmiEditorSymbolRegistry **out_registry);
/**
 * Release or reset state held by editor symbol registry so the same storage can be reused
 * safely.
 */
void umi_editor_symbol_registry_destroy(UmiEditorSymbolRegistry *registry);
/**
 * Provide the editor symbol registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_symbol_registry_upsert(UmiEditorSymbolRegistry *registry,const UmiEditorSymbolSnapshot *item);
/**
 * Remove editor symbol registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_symbol_registry_remove(UmiEditorSymbolRegistry *registry,const char *id);
/**
 * Find editor symbol registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_symbol_registry_find(const UmiEditorSymbolRegistry *registry,const char *id,UmiEditorSymbolSnapshot *out_item);
/**
 * Find editor symbol registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_symbol_registry_at(const UmiEditorSymbolRegistry *registry,size_t index,UmiEditorSymbolSnapshot *out_item);
/**
 * Return the number of records represented by editor symbol registry without changing
 * their state.
 */
size_t umi_editor_symbol_registry_count(const UmiEditorSymbolRegistry *registry);
/**
 * Provide the editor symbol registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_editor_symbol_registry_revision(const UmiEditorSymbolRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
