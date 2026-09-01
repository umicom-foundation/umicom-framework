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
typedef struct UmiEditorSymbolRegistry UmiEditorSymbolRegistry;
UmiStatus umi_editor_symbol_registry_create(UmiEditorSymbolRegistry **out_registry);
void umi_editor_symbol_registry_destroy(UmiEditorSymbolRegistry *registry);
UmiStatus umi_editor_symbol_registry_upsert(UmiEditorSymbolRegistry *registry,const UmiEditorSymbolSnapshot *item);
UmiStatus umi_editor_symbol_registry_remove(UmiEditorSymbolRegistry *registry,const char *id);
UmiStatus umi_editor_symbol_registry_find(const UmiEditorSymbolRegistry *registry,const char *id,UmiEditorSymbolSnapshot *out_item);
UmiStatus umi_editor_symbol_registry_at(const UmiEditorSymbolRegistry *registry,size_t index,UmiEditorSymbolSnapshot *out_item);
size_t umi_editor_symbol_registry_count(const UmiEditorSymbolRegistry *registry);
uint64_t umi_editor_symbol_registry_revision(const UmiEditorSymbolRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
