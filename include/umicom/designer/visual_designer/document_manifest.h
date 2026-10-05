/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/document_manifest.h
 *
 * PURPOSE:
 *   Summarise the pages, forms, components and bindings in a visual document.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_DOCUMENT_MANIFEST_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_DOCUMENT_MANIFEST_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer document manifest data shared with callers of this public contract.
 */
typedef struct UmiRadDocumentManifest {
    char application_id[UMI_RAD_ID_CAPACITY];
    size_t page_count;
    size_t form_count;
    size_t component_count;
    size_t binding_count;
    uint64_t revision;
} UmiRadDocumentManifest;
/**
 * Initialise visual designer document manifest from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_document_manifest_init(UmiRadDocumentManifest *item);
/**
 * Check that visual designer document manifest satisfies its contract before another service relies on
 * it.
 */
int umi_rad_document_manifest_is_valid(const UmiRadDocumentManifest *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_document_manifest_archive_encode(const UmiRadDocumentManifest *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_document_manifest_archive_decode(const void *bytes, size_t byte_count,
    UmiRadDocumentManifest *value);

#ifdef __cplusplus
}
#endif
#endif
