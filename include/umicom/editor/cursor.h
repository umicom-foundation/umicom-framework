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

/* BEGINNER NOTE:
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
typedef struct UmiEditorCursorRegistry UmiEditorCursorRegistry;
UmiStatus umi_editor_cursor_registry_create(UmiEditorCursorRegistry **out_registry);
void umi_editor_cursor_registry_destroy(UmiEditorCursorRegistry *registry);
UmiStatus umi_editor_cursor_registry_upsert(UmiEditorCursorRegistry *registry,const UmiEditorCursorSnapshot *item);
UmiStatus umi_editor_cursor_registry_remove(UmiEditorCursorRegistry *registry,const char *id);
UmiStatus umi_editor_cursor_registry_find(const UmiEditorCursorRegistry *registry,const char *id,UmiEditorCursorSnapshot *out_item);
UmiStatus umi_editor_cursor_registry_at(const UmiEditorCursorRegistry *registry,size_t index,UmiEditorCursorSnapshot *out_item);
size_t umi_editor_cursor_registry_count(const UmiEditorCursorRegistry *registry);
uint64_t umi_editor_cursor_registry_revision(const UmiEditorCursorRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
