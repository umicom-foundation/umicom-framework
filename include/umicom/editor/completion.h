/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/completion.h
 *
 * PURPOSE:
 *   Define code-completion items reusable by native and Language Server providers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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
typedef struct UmiEditorCompletionRegistry UmiEditorCompletionRegistry;
UmiStatus umi_editor_completion_registry_create(UmiEditorCompletionRegistry **out_registry);
void umi_editor_completion_registry_destroy(UmiEditorCompletionRegistry *registry);
UmiStatus umi_editor_completion_registry_upsert(UmiEditorCompletionRegistry *registry,const UmiEditorCompletionSnapshot *item);
UmiStatus umi_editor_completion_registry_remove(UmiEditorCompletionRegistry *registry,const char *id);
UmiStatus umi_editor_completion_registry_find(const UmiEditorCompletionRegistry *registry,const char *id,UmiEditorCompletionSnapshot *out_item);
UmiStatus umi_editor_completion_registry_at(const UmiEditorCompletionRegistry *registry,size_t index,UmiEditorCompletionSnapshot *out_item);
size_t umi_editor_completion_registry_count(const UmiEditorCompletionRegistry *registry);
uint64_t umi_editor_completion_registry_revision(const UmiEditorCompletionRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
