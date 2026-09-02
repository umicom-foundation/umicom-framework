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

/*
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
/**
 * Represent the editor marker snapshot data shared with callers of this public contract.
 */
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
/**
 * Represent the editor marker registry data shared with callers of this public contract.
 */
typedef struct UmiEditorMarkerRegistry UmiEditorMarkerRegistry;
/**
 * Initialise editor marker registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_marker_registry_create(UmiEditorMarkerRegistry **out_registry);
/**
 * Release or reset state held by editor marker registry so the same storage can be reused
 * safely.
 */
void umi_editor_marker_registry_destroy(UmiEditorMarkerRegistry *registry);
/**
 * Provide the editor marker registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_marker_registry_upsert(UmiEditorMarkerRegistry *registry,const UmiEditorMarkerSnapshot *item);
/**
 * Remove editor marker registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_marker_registry_remove(UmiEditorMarkerRegistry *registry,const char *id);
/**
 * Find editor marker registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_marker_registry_find(const UmiEditorMarkerRegistry *registry,const char *id,UmiEditorMarkerSnapshot *out_item);
/**
 * Find editor marker registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_marker_registry_at(const UmiEditorMarkerRegistry *registry,size_t index,UmiEditorMarkerSnapshot *out_item);
/**
 * Return the number of records represented by editor marker registry without changing
 * their state.
 */
size_t umi_editor_marker_registry_count(const UmiEditorMarkerRegistry *registry);
/**
 * Provide the editor marker registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_editor_marker_registry_revision(const UmiEditorMarkerRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
