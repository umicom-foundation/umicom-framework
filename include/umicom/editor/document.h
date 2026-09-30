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
#include "umicom/base/snapshot_validation.h"
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

/** Check id and all fixed text arrays before string lookup or publication.
 * id must be nonempty; optional text may be empty. Unterminated arrays return
 * INVALID_ARGUMENT with optional static field diagnostics. No allocation or
 * mutation occurs. This checks text bounds, not UTF-8 or domain semantics.
 * Single-record upsert uses the same preflight and rejects revision overflow. */
UmiStatus umi_editor_document_snapshot_validate(const UmiEditorDocumentSnapshot *item,
    UmiSnapshotValidation *outValidation);

/** Insert/replace up to UMI_EDITOR_DOCUMENT_CAPACITY distinct IDs as one in-memory operation.
 * Duplicate input IDs are rejected; stored IDs can be replaced. Valid records
 * retain ordinary upsert order, normalisation and revision increments. Any
 * failure leaves the live registry unchanged. Empty input is a successful no-op.
 * A full private registry is allocated temporarily. No files or callbacks are
 * used. Keep all access on the owner's thread; this is not thread locking.
 * Inputs are borrowed for this call, unchanged and never retained. outResult
 * is optional, written on every return, and must not overlap input or registry. */
UmiStatus umi_editor_document_registry_upsert_many(UmiEditorDocumentRegistry *registry,
    const UmiEditorDocumentSnapshot *items, size_t count, UmiSnapshotBatchResult *outResult);

#ifdef __cplusplus
}
#endif
#endif
