/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/document_manifest.h
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
#ifndef UMICOM_DESIGNER_RAD_DOCUMENT_MANIFEST_H
#define UMICOM_DESIGNER_RAD_DOCUMENT_MANIFEST_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad document manifest data shared with callers of this public contract.
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
 * Initialise rad document manifest from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_document_manifest_init(UmiRadDocumentManifest *item);
/**
 * Check that rad document manifest satisfies its contract before another service relies on
 * it.
 */
int umi_rad_document_manifest_is_valid(const UmiRadDocumentManifest *item);
#ifdef __cplusplus
}
#endif
#endif
