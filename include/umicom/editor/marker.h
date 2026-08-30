/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/marker.h
 *
 * PURPOSE:
 *   Define reusable editor markers for bookmarks, breakpoints, search hits and diagnostics.
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
#ifndef UMICOM_EDITOR_MARKER_H
#define UMICOM_EDITOR_MARKER_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_MARKER_CAPACITY 4096U
typedef struct UmiEditorMarkerSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char kind[64];
    char label[256];
    uint64_t line;
    uint64_t column;
    int severity;
    int enabled;
    uint64_t revision;
} UmiEditorMarkerSnapshot;
typedef struct UmiEditorMarkerRegistry UmiEditorMarkerRegistry;
UmiStatus umi_editor_marker_registry_create(UmiEditorMarkerRegistry **out_registry);
void umi_editor_marker_registry_destroy(UmiEditorMarkerRegistry *registry);
UmiStatus umi_editor_marker_registry_upsert(UmiEditorMarkerRegistry *registry,const UmiEditorMarkerSnapshot *item);
UmiStatus umi_editor_marker_registry_remove(UmiEditorMarkerRegistry *registry,const char *id);
UmiStatus umi_editor_marker_registry_find(const UmiEditorMarkerRegistry *registry,const char *id,UmiEditorMarkerSnapshot *out_item);
UmiStatus umi_editor_marker_registry_at(const UmiEditorMarkerRegistry *registry,size_t index,UmiEditorMarkerSnapshot *out_item);
size_t umi_editor_marker_registry_count(const UmiEditorMarkerRegistry *registry);
uint64_t umi_editor_marker_registry_revision(const UmiEditorMarkerRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
