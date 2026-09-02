/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/completion.h
 *
 * PURPOSE:
 *   Define code-completion items reusable by native and Language Server providers.
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
#ifndef UMICOM_EDITOR_COMPLETION_H
#define UMICOM_EDITOR_COMPLETION_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_COMPLETION_CAPACITY 8192U
/**
 * Represent the editor completion snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorCompletionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char label[256];
    char detail[512];
    char insert_text[1024];
    char kind[64];
    char sort_text[256];
    char filter_text[256];
    int deprecated;
    uint64_t revision;
} UmiEditorCompletionSnapshot;
/**
 * Represent the editor completion registry data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorCompletionRegistry UmiEditorCompletionRegistry;
/**
 * Initialise editor completion registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_completion_registry_create(UmiEditorCompletionRegistry **out_registry);
/**
 * Release or reset state held by editor completion registry so the same storage can be
 * reused safely.
 */
void umi_editor_completion_registry_destroy(UmiEditorCompletionRegistry *registry);
/**
 * Provide the editor completion registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_completion_registry_upsert(UmiEditorCompletionRegistry *registry,const UmiEditorCompletionSnapshot *item);
/**
 * Remove editor completion registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_completion_registry_remove(UmiEditorCompletionRegistry *registry,const char *id);
/**
 * Find editor completion registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_completion_registry_find(const UmiEditorCompletionRegistry *registry,const char *id,UmiEditorCompletionSnapshot *out_item);
/**
 * Find editor completion registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_completion_registry_at(const UmiEditorCompletionRegistry *registry,size_t index,UmiEditorCompletionSnapshot *out_item);
/**
 * Return the number of records represented by editor completion registry without changing
 * their state.
 */
size_t umi_editor_completion_registry_count(const UmiEditorCompletionRegistry *registry);
/**
 * Provide the editor completion registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_completion_registry_revision(const UmiEditorCompletionRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
