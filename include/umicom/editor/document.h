/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/document.h
 *
 * PURPOSE:
 *   Define editor-document metadata independent of the text storage implementation.
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
#ifndef UMICOM_EDITOR_DOCUMENT_H
#define UMICOM_EDITOR_DOCUMENT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_DOCUMENT_CAPACITY 1024U
/**
 * Represent the editor document snapshot data shared with callers of this public contract.
 */
typedef struct UmiEditorDocumentSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char uri[1024];
    char language_id[128];
    char title[256];
    uint64_t version;
    uint64_t byte_count;
    uint64_t line_count;
    int dirty;
    int read_only;
    uint64_t revision;
} UmiEditorDocumentSnapshot;
/**
 * Represent the editor document registry data shared with callers of this public contract.
 */
typedef struct UmiEditorDocumentRegistry UmiEditorDocumentRegistry;
/**
 * Initialise editor document registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_document_registry_create(UmiEditorDocumentRegistry **out_registry);
/**
 * Release or reset state held by editor document registry so the same storage can be
 * reused safely.
 */
void umi_editor_document_registry_destroy(UmiEditorDocumentRegistry *registry);
/**
 * Provide the editor document registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_document_registry_upsert(UmiEditorDocumentRegistry *registry,const UmiEditorDocumentSnapshot *item);
/**
 * Remove editor document registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_document_registry_remove(UmiEditorDocumentRegistry *registry,const char *id);
/**
 * Find editor document registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_document_registry_find(const UmiEditorDocumentRegistry *registry,const char *id,UmiEditorDocumentSnapshot *out_item);
/**
 * Find editor document registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_document_registry_at(const UmiEditorDocumentRegistry *registry,size_t index,UmiEditorDocumentSnapshot *out_item);
/**
 * Return the number of records represented by editor document registry without changing
 * their state.
 */
size_t umi_editor_document_registry_count(const UmiEditorDocumentRegistry *registry);
/**
 * Provide the editor document registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_document_registry_revision(const UmiEditorDocumentRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
