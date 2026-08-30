/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/clipboard.h
 *
 * PURPOSE:
 *   Define a toolkit-neutral designer clipboard for copy, cut, paste and duplication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_DESIGNER_CLIPBOARD_H
#define UMICOM_DESIGNER_CLIPBOARD_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_CLIPBOARD_CAPACITY 256U

typedef struct UmiDesignerClipboardItemSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char source_node_id[128];
    char component_type[128];
    char serialized[2048];
    uint64_t copied_at;
    uint64_t revision;
} UmiDesignerClipboardItemSnapshot;

typedef struct UmiDesignerClipboardItemRegistry UmiDesignerClipboardItemRegistry;

UmiStatus umi_designer_clipboard_registry_create(UmiDesignerClipboardItemRegistry **out_registry);
void umi_designer_clipboard_registry_destroy(UmiDesignerClipboardItemRegistry *registry);
UmiStatus umi_designer_clipboard_registry_upsert(UmiDesignerClipboardItemRegistry *registry, const UmiDesignerClipboardItemSnapshot *item);
UmiStatus umi_designer_clipboard_registry_remove(UmiDesignerClipboardItemRegistry *registry, const char *id);
UmiStatus umi_designer_clipboard_registry_find(const UmiDesignerClipboardItemRegistry *registry, const char *id, UmiDesignerClipboardItemSnapshot *out_item);
UmiStatus umi_designer_clipboard_registry_at(const UmiDesignerClipboardItemRegistry *registry, size_t index, UmiDesignerClipboardItemSnapshot *out_item);
size_t umi_designer_clipboard_registry_count(const UmiDesignerClipboardItemRegistry *registry);
uint64_t umi_designer_clipboard_registry_revision(const UmiDesignerClipboardItemRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
