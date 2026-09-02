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

/*
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

/**
 * Represent the designer clipboard item snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the designer clipboard item registry data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerClipboardItemRegistry UmiDesignerClipboardItemRegistry;

/**
 * Initialise designer clipboard registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_designer_clipboard_registry_create(UmiDesignerClipboardItemRegistry **out_registry);
/**
 * Release or reset state held by designer clipboard registry so the same storage can be
 * reused safely.
 */
void umi_designer_clipboard_registry_destroy(UmiDesignerClipboardItemRegistry *registry);
/**
 * Provide the designer clipboard registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_clipboard_registry_upsert(UmiDesignerClipboardItemRegistry *registry, const UmiDesignerClipboardItemSnapshot *item);
/**
 * Remove designer clipboard registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_designer_clipboard_registry_remove(UmiDesignerClipboardItemRegistry *registry, const char *id);
/**
 * Find designer clipboard registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_designer_clipboard_registry_find(const UmiDesignerClipboardItemRegistry *registry, const char *id, UmiDesignerClipboardItemSnapshot *out_item);
/**
 * Find designer clipboard registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_designer_clipboard_registry_at(const UmiDesignerClipboardItemRegistry *registry, size_t index, UmiDesignerClipboardItemSnapshot *out_item);
/**
 * Return the number of records represented by designer clipboard registry without changing
 * their state.
 */
size_t umi_designer_clipboard_registry_count(const UmiDesignerClipboardItemRegistry *registry);
/**
 * Provide the designer clipboard registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_designer_clipboard_registry_revision(const UmiDesignerClipboardItemRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
