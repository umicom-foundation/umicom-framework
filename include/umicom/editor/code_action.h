/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/code_action.h
 *
 * PURPOSE:
 *   Define editor code actions that execute through canonical command IDs.
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
#ifndef UMICOM_EDITOR_CODE_ACTION_H
#define UMICOM_EDITOR_CODE_ACTION_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_CODE_ACTION_CAPACITY 4096U
typedef struct UmiEditorCodeActionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char document_id[128];
    char title[512];
    char kind[128];
    char command_id[128];
    char argument[1024];
    int preferred;
    int enabled;
    uint64_t revision;
} UmiEditorCodeActionSnapshot;
typedef struct UmiEditorCodeActionRegistry UmiEditorCodeActionRegistry;
UmiStatus umi_editor_code_action_registry_create(UmiEditorCodeActionRegistry **out_registry);
void umi_editor_code_action_registry_destroy(UmiEditorCodeActionRegistry *registry);
UmiStatus umi_editor_code_action_registry_upsert(UmiEditorCodeActionRegistry *registry,const UmiEditorCodeActionSnapshot *item);
UmiStatus umi_editor_code_action_registry_remove(UmiEditorCodeActionRegistry *registry,const char *id);
UmiStatus umi_editor_code_action_registry_find(const UmiEditorCodeActionRegistry *registry,const char *id,UmiEditorCodeActionSnapshot *out_item);
UmiStatus umi_editor_code_action_registry_at(const UmiEditorCodeActionRegistry *registry,size_t index,UmiEditorCodeActionSnapshot *out_item);
size_t umi_editor_code_action_registry_count(const UmiEditorCodeActionRegistry *registry);
uint64_t umi_editor_code_action_registry_revision(const UmiEditorCodeActionRegistry *registry);
#ifdef __cplusplus
}
#endif
#endif
