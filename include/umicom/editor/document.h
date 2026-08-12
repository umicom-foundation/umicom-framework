/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/document.h
 *
 * PURPOSE:
 *   Define editor-document metadata independent of the text storage implementation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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
typedef struct UmiEditorDocumentRegistry UmiEditorDocumentRegistry;
UmiStatus umi_editor_document_registry_create(UmiEditorDocumentRegistry **out_registry);
void umi_editor_document_registry_destroy(UmiEditorDocumentRegistry *registry);
UmiStatus umi_editor_document_registry_upsert(UmiEditorDocumentRegistry *registry,const UmiEditorDocumentSnapshot *item);
UmiStatus umi_editor_document_registry_remove(UmiEditorDocumentRegistry *registry,const char *id);
UmiStatus umi_editor_document_registry_find(const UmiEditorDocumentRegistry *registry,const char *id,UmiEditorDocumentSnapshot *out_item);
UmiStatus umi_editor_document_registry_at(const UmiEditorDocumentRegistry *registry,size_t index,UmiEditorDocumentSnapshot *out_item);
size_t umi_editor_document_registry_count(const UmiEditorDocumentRegistry *registry);
uint64_t umi_editor_document_registry_revision(const UmiEditorDocumentRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
