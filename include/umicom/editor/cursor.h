/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/cursor.h
 *
 * PURPOSE:
 *   Define reusable editor cursor and caret state.
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
#ifndef UMICOM_EDITOR_CURSOR_H
#define UMICOM_EDITOR_CURSOR_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_CURSOR_CAPACITY 2048U
/**
 * Represent the editor cursor snapshot data shared with callers of this public contract.
 */
typedef struct UmiEditorCursorSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    uint64_t line;
    uint64_t column;
    uint64_t preferred_column;
    int primary;
    int visible;
    uint64_t revision;
} UmiEditorCursorSnapshot;
/**
 * Represent the editor cursor registry data shared with callers of this public contract.
 */
typedef struct UmiEditorCursorRegistry UmiEditorCursorRegistry;
/**
 * Initialise editor cursor registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_cursor_registry_create(UmiEditorCursorRegistry **out_registry);
/**
 * Release or reset state held by editor cursor registry so the same storage can be reused
 * safely.
 */
void umi_editor_cursor_registry_destroy(UmiEditorCursorRegistry *registry);
/**
 * Provide the editor cursor registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_cursor_registry_upsert(UmiEditorCursorRegistry *registry,const UmiEditorCursorSnapshot *item);
/**
 * Remove editor cursor registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_cursor_registry_remove(UmiEditorCursorRegistry *registry,const char *id);
/**
 * Find editor cursor registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_cursor_registry_find(const UmiEditorCursorRegistry *registry,const char *id,UmiEditorCursorSnapshot *out_item);
/**
 * Find editor cursor registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_cursor_registry_at(const UmiEditorCursorRegistry *registry,size_t index,UmiEditorCursorSnapshot *out_item);
/**
 * Return the number of records represented by editor cursor registry without changing
 * their state.
 */
size_t umi_editor_cursor_registry_count(const UmiEditorCursorRegistry *registry);
/**
 * Provide the editor cursor registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_editor_cursor_registry_revision(const UmiEditorCursorRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
